#pragma once

#include "actor.h"
#include "box2d/b2_fixture.h"
#include "box2d/b2_math.h"
#include "box2d/b2_world_callbacks.h"
#include "physics/rigidbody.h"

#include "luabindings.h"
#include <algorithm>
#include <vector>

struct HitResult {
    Actor *actor = nullptr;
    b2Vec2 point = b2Vec2(0.0F, 0.0F);
    b2Vec2 normal = b2Vec2(0.0F, 0.0F);
    bool is_trigger = false;
    float fraction = 0.0F; // internal for sorting nearest-first
};

class RaycastClosestCallback : public b2RayCastCallback {
public:
    bool has_hit = false;
    HitResult hit{};

    float ReportFixture(b2Fixture *fixture, const b2Vec2 &point,
                        const b2Vec2 &normal, float fraction) override {
        if (fixture == nullptr) {
            return -1.0F;
        }

        // TODO: if you add explicit phantom fixture metadata, filter it here.

        auto *actor = reinterpret_cast<Actor *>(fixture->GetUserData().pointer);
        if (actor == nullptr) {
            return -1.0F;
        }

        has_hit = true;
        hit.actor = actor;
        hit.point = point;
        hit.normal = normal;
        hit.is_trigger = fixture->IsSensor();
        hit.fraction = fraction;

        // Clip ray to this point so farther fixtures are skipped.
        return fraction;
    }
};

class RaycastAllCallback : public b2RayCastCallback {
public:
    std::vector<HitResult> hits;

    float ReportFixture(b2Fixture *fixture, const b2Vec2 &point,
                        const b2Vec2 &normal, float fraction) override {
        if (fixture == nullptr) {
            return -1.0F;
        }

        // TODO: if you add explicit phantom fixture metadata, filter it here.

        auto *actor = reinterpret_cast<Actor *>(fixture->GetUserData().pointer);
        if (actor == nullptr) {
            return -1.0F;
        }

        HitResult hit;
        hit.actor = actor;
        hit.point = point;
        hit.normal = normal;
        hit.is_trigger = fixture->IsSensor();
        hit.fraction = fraction;
        hits.push_back(hit);

        // Continue through all fixtures.
        return 1.0F;
    }
};

inline luabridge::LuaRef physicsRaycast(b2Vec2 pos, b2Vec2 dir, float dist) {
    lua_State *L = LuaBindings::getState();
    if (dist <= 0.0F) {
        return luabridge::LuaRef(L);
    }

    if (dir.LengthSquared() <= 0.0F) {
        return luabridge::LuaRef(L);
    }

    dir.Normalize();
    const b2Vec2 end = pos + (dist * dir);

    RaycastClosestCallback callback;
    Rigidbody::getWorld().RayCast(&callback, pos, end);

    if (!callback.has_hit) {
        return luabridge::LuaRef(L);
    }

    return luabridge::LuaRef(L, callback.hit);
}

inline luabridge::LuaRef physicsRaycastAll(b2Vec2 pos, b2Vec2 dir, float dist) {
    lua_State *L = LuaBindings::getState();
    luabridge::LuaRef table = luabridge::newTable(L);

    if (dist <= 0.0F) {
        return table;
    }

    if (dir.LengthSquared() <= 0.0F) {
        return table;
    }

    dir.Normalize();
    const b2Vec2 end = pos + (dist * dir);

    RaycastAllCallback callback;
    Rigidbody::getWorld().RayCast(&callback, pos, end);

    std::vector<HitResult> out = std::move(callback.hits);
    std::sort(out.begin(), out.end(),
              [](const HitResult &a, const HitResult &b) {
                  return a.fraction < b.fraction;
              });

    for (size_t i = 0; i < out.size(); ++i) {
        table[static_cast<int>(i + 1)] = out[i];
    }
    return table;
}
