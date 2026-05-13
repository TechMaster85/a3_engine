#include "luabindings.h"

#include "audio/audio.h"
#include "core/fileutil.h"
#include "engine.h"
#include "events/eventmanager.h"
#include "input/input.h"
#include "particles/particlesystem.h"
#include "physics/raycasting.h"
#include "physics/rigidbody.h"
#include "rendering/renderer.h"
#include "scene/actor.h"
#include "scene/scenedb.h"

#include <lua/lua.hpp>

#include <LuaBridge/LuaBridge.h>
#include <box2d/b2_math.h>

#include <iostream>

// Namespaces not worth making extra classes for
namespace {
namespace Debug {
void log(const luabridge::LuaRef &message) {
    if (message.isNil()) {
        std::cout << '\n';
    } else {
        std::cout << message.tostring() << '\n';
    }
}
} // namespace Debug

namespace Vector2Binding {
b2Vec2 add(const b2Vec2 *self, const b2Vec2 &other) { return *self + other; }
b2Vec2 sub(const b2Vec2 *self, const b2Vec2 &other) { return *self - other; }
b2Vec2 mul(const b2Vec2 *self, float s) { return {self->x * s, self->y * s}; }
} // namespace Vector2Binding
} // namespace

void LuaBindings::registerAll(lua_State *L) {
    luabridge::setGlobal(L, FileUtil::RESOURCES_PATH.string(),
                         "resources_directory");

    luabridge::getGlobalNamespace(L)
        .beginClass<glm::vec2>("vec2")
        .addProperty("x", &glm::vec2::x)
        .addProperty("y", &glm::vec2::y)
        .endClass();

    luabridge::getGlobalNamespace(L)
        .beginNamespace("Debug")
        .addFunction("Log", &Debug::log)
        .endNamespace();

    luabridge::getGlobalNamespace(L)
        .beginNamespace("Application")
        .addFunction("Quit", []() { exit(0); })
        .addFunction("Sleep", &Engine::sleep)
        .addFunction("GetFrame", &Engine::getFrameNumber)
        .addFunction("OpenURL", &Engine::openUrl)
        .endNamespace();

    Actor::createLuaBindings(L);
    SceneDB::createLuaBindings(L);
    Input::registerLuaBindings(L);
    Audio::registerLuaBindings(L);
    Renderer::registerLuaBindings(L);

    luabridge::getGlobalNamespace(L)
        .beginClass<Rigidbody>("Rigidbody")
        .addFunction("OnStart", &Rigidbody::onStart)
        .addFunction("OnDestroy", &Rigidbody::onDestroy)
        .addProperty("enabled", &Rigidbody::enabled)
        .addProperty("key", &Rigidbody::getKey)
        .addProperty("type", &Rigidbody::getType)
        .addProperty("actor", &Rigidbody::getActor)
        .addProperty("x", &Rigidbody::getX, &Rigidbody::setX)
        .addProperty("y", &Rigidbody::getY, &Rigidbody::setY)
        .addProperty("rotation", &Rigidbody::getRotationDeg,
                     &Rigidbody::setRotationDeg)
        .addProperty("body_type", &Rigidbody::getBodyType,
                     &Rigidbody::setBodyType)
        .addProperty("precise", &Rigidbody::getPrecise, &Rigidbody::setPrecise)
        .addProperty("gravity_scale", &Rigidbody::getGravityScaleVal,
                     &Rigidbody::setGravityScaleVal)
        .addProperty("density", &Rigidbody::getDensity, &Rigidbody::setDensity)
        .addProperty("angular_friction", &Rigidbody::getAngularFriction,
                     &Rigidbody::setAngularFriction)
        .addProperty("has_collider", &Rigidbody::getHasCollider,
                     &Rigidbody::setHasCollider)
        .addProperty("has_trigger", &Rigidbody::getHasTrigger,
                     &Rigidbody::setHasTrigger)
        .addProperty("collider_type", &Rigidbody::getColliderType,
                     &Rigidbody::setColliderType)
        .addProperty("width", &Rigidbody::getWidth, &Rigidbody::setWidth)
        .addProperty("height", &Rigidbody::getHeight, &Rigidbody::setHeight)
        .addProperty("radius", &Rigidbody::getRadius, &Rigidbody::setRadius)
        .addProperty("friction", &Rigidbody::getFriction,
                     &Rigidbody::setFriction)
        .addProperty("bounciness", &Rigidbody::getBounciness,
                     &Rigidbody::setBounciness)
        .addProperty("trigger_type", &Rigidbody::getTriggerType,
                     &Rigidbody::setTriggerType)
        .addProperty("trigger_width", &Rigidbody::getTriggerWidth,
                     &Rigidbody::setTriggerWidth)
        .addProperty("trigger_height", &Rigidbody::getTriggerHeight,
                     &Rigidbody::setTriggerHeight)
        .addProperty("trigger_radius", &Rigidbody::getTriggerRadius,
                     &Rigidbody::setTriggerRadius)
        .addFunction("GetPosition", &Rigidbody::getPosition)
        .addFunction("GetRotation", &Rigidbody::getRotation)
        .addFunction("AddForce", &Rigidbody::addForce)
        .addFunction("SetVelocity", &Rigidbody::setVelocity)
        .addFunction("SetPosition", &Rigidbody::setPosition)
        .addFunction("SetRotation", &Rigidbody::setRotation)
        .addFunction("SetAngularVelocity", &Rigidbody::setAngularVelocity)
        .addFunction("SetGravityScale", &Rigidbody::setGravityScale)
        .addFunction("SetUpDirection", &Rigidbody::setUpDirection)
        .addFunction("SetRightDirection", &Rigidbody::setRightDirection)
        .addFunction("GetVelocity", &Rigidbody::getVelocity)
        .addFunction("GetAngularVelocity", &Rigidbody::getAngularVelocity)
        .addFunction("GetGravityScale", &Rigidbody::getGravityScale)
        .addFunction("GetUpDirection", &Rigidbody::getUpDirection)
        .addFunction("GetRightDirection", &Rigidbody::getRightDirection)
        .endClass();

    luabridge::getGlobalNamespace(L)
        .beginClass<b2Vec2>("Vector2")
        .addConstructor<void (*)(float, float)>()
        .addProperty("x", &b2Vec2::x)
        .addProperty("y", &b2Vec2::y)
        .addFunction("__add", &Vector2Binding::add)
        .addFunction("__sub", &Vector2Binding::sub)
        .addFunction("__mul", &Vector2Binding::mul)
        .addFunction("Normalize", &b2Vec2::Normalize)
        .addFunction("Length", &b2Vec2::Length)
        .endClass();

    luabridge::getGlobalNamespace(L)
        .beginNamespace("Vector2")
        .addFunction(
            "Distance",
            static_cast<float (*)(const b2Vec2 &, const b2Vec2 &)>(&b2Distance))
        .addFunction(
            "Dot",
            static_cast<float (*)(const b2Vec2 &, const b2Vec2 &)>(&b2Dot))
        .endNamespace();

    luabridge::getGlobalNamespace(L)
        .beginClass<Collision>("Collision")
        .addProperty("other", &Collision::other)
        .addProperty("point", &Collision::point)
        .addProperty("relative_velocity", &Collision::relative_velocity)
        .addProperty("normal", &Collision::normal)
        .endClass();

    luabridge::getGlobalNamespace(L)
        .beginClass<HitResult>("HitResult")
        .addProperty("actor", &HitResult::actor)
        .addProperty("point", &HitResult::point)
        .addProperty("normal", &HitResult::normal)
        .addProperty("is_trigger", &HitResult::is_trigger)
        .endClass();

    luabridge::getGlobalNamespace(L)
        .beginNamespace("Physics")
        .addFunction("Raycast", &physicsRaycast)
        .addFunction("RaycastAll", &physicsRaycastAll)
        .endNamespace();

    EventManager::registerLuaBindings(L);

    luabridge::getGlobalNamespace(L)
        .beginClass<ParticleSystem>("ParticleSystem")
        .addFunction("OnUpdate", &ParticleSystem::onUpdate)
        .addFunction("Stop", &ParticleSystem::stop)
        .addFunction("Play", &ParticleSystem::play)
        .addFunction("Burst", &ParticleSystem::burst)
        .addProperty("enabled", &ParticleSystem::enabled)
        .addProperty("type", &ParticleSystem::getType)
        .addProperty("x", &ParticleSystem::getX, &ParticleSystem::setX)
        .addProperty("y", &ParticleSystem::getY, &ParticleSystem::setY)
        .addProperty("start_color_r", &ParticleSystem::getStartColorR,
                     &ParticleSystem::setStartColorR)
        .addProperty("start_color_g", &ParticleSystem::getStartColorG,
                     &ParticleSystem::setStartColorG)
        .addProperty("start_color_b", &ParticleSystem::getStartColorB,
                     &ParticleSystem::setStartColorB)
        .endClass();
}
