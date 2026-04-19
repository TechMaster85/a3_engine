#pragma once

#include "box2d/b2_contact.h"
#include "box2d/b2_math.h"
#include "box2d/b2_world_callbacks.h"

#include "physics/collision.h"

static const b2Vec2 CONTACT_LISTENER_SENTINEL = b2Vec2(-999.0F, -999.0F);

class ContactListener : public b2ContactListener {
public:
    void BeginContact(b2Contact *contact) override {
        Collision collisionA;
        Collision collisionB;

        b2Fixture *fixtureA = contact->GetFixtureA();
        b2Fixture *fixtureB = contact->GetFixtureB();

        auto *actorA =
            reinterpret_cast<Actor *>(fixtureA->GetUserData().pointer);
        auto *actorB =
            reinterpret_cast<Actor *>(fixtureB->GetUserData().pointer);

        collisionA.other = actorB;
        collisionB.other = actorA;

        const b2Vec2 relativeVelocity =
            fixtureA->GetBody()->GetLinearVelocity() -
            fixtureB->GetBody()->GetLinearVelocity();

        collisionA.relative_velocity = relativeVelocity;
        collisionB.relative_velocity = relativeVelocity;

        b2WorldManifold worldManifold;
        contact->GetWorldManifold(&worldManifold);
        collisionA.point = worldManifold.points[0];
        collisionA.normal = worldManifold.normal;
        collisionB.point = worldManifold.points[0];
        collisionB.normal = worldManifold.normal;

        if (!fixtureA->IsSensor() && !fixtureB->IsSensor()) {
            ContactQueue::collisions.push_back({actorA, true, collisionA});
            ContactQueue::collisions.push_back({actorB, true, collisionB});
        }

        if (fixtureA->IsSensor() && fixtureB->IsSensor()) {
            collisionA.point = CONTACT_LISTENER_SENTINEL;
            collisionA.normal = CONTACT_LISTENER_SENTINEL;
            collisionB.point = CONTACT_LISTENER_SENTINEL;
            collisionB.normal = CONTACT_LISTENER_SENTINEL;

            ContactQueue::triggers.push_back({actorA, true, collisionA});
            ContactQueue::triggers.push_back({actorB, true, collisionB});
        }
    }

    void EndContact(b2Contact *contact) override {
        Collision collisionA;
        Collision collisionB;

        b2Fixture *fixtureA = contact->GetFixtureA();
        b2Fixture *fixtureB = contact->GetFixtureB();

        auto *actorA =
            reinterpret_cast<Actor *>(fixtureA->GetUserData().pointer);
        auto *actorB =
            reinterpret_cast<Actor *>(fixtureB->GetUserData().pointer);

        collisionA.other = actorB;
        collisionB.other = actorA;

        const b2Vec2 relativeVelocity =
            fixtureA->GetBody()->GetLinearVelocity() -
            fixtureB->GetBody()->GetLinearVelocity();

        collisionA.relative_velocity = relativeVelocity;
        collisionB.relative_velocity = relativeVelocity;

        collisionA.point = CONTACT_LISTENER_SENTINEL;
        collisionA.normal = CONTACT_LISTENER_SENTINEL;
        collisionB.point = CONTACT_LISTENER_SENTINEL;
        collisionB.normal = CONTACT_LISTENER_SENTINEL;

        if (!fixtureA->IsSensor() && !fixtureB->IsSensor()) {
            ContactQueue::collisions.push_back({actorA, false, collisionA});
            ContactQueue::collisions.push_back({actorB, false, collisionB});
        }

        if (fixtureA->IsSensor() && fixtureB->IsSensor()) {
            ContactQueue::triggers.push_back({actorA, false, collisionA});
            ContactQueue::triggers.push_back({actorB, false, collisionB});
        }
    }
};
