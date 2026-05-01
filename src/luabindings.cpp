#include "luabindings.h"

#include "Helper.h"
#include "LuaBridge/detail/Namespace.h"
#include "actor.h"
#include "audio.h"
#include "box2d/b2_math.h"
#include "core/filepath.h"
#include "events/eventmanager.h"
#include "input/input.h"
#include "particles/particlesystem.h"
#include "physics/raycasting.h"
#include "physics/rigidbody.h"
#include "rendering/renderer.h"
#include "scenedb.h"

#include <chrono>
#include <iostream>
#include <thread>

lua_State *LuaBindings::state = nullptr;

lua_State *LuaBindings::getState() { return state; }

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

namespace Application {
void quit() { exit(0); }
void sleep(int x) { std::this_thread::sleep_for(std::chrono::milliseconds(x)); }
int getFrame() { return Helper::GetFrameNumber(); }
void openUrl(const std::string &url) {
#ifdef _WIN32
    const std::string command = "start " + url;
#elif defined(__APPLE__)
    const std::string command = "open " + url;
#else
    const std::string command = "xdg-open " + url;
#endif
    std::system(command.c_str());
}

} // namespace Application

} // namespace

// Extra bindings too cumbersome to write in the property directly
namespace {
namespace Vector2Binding {
b2Vec2 add(const b2Vec2 *self, const b2Vec2 &other) { return *self + other; }
b2Vec2 sub(const b2Vec2 *self, const b2Vec2 &other) { return *self - other; }
b2Vec2 mul(const b2Vec2 *self, float s) { return {self->x * s, self->y * s}; }
} // namespace Vector2Binding
} // namespace

LuaBindings::LuaBindings(lua_State *L) {
    state = L;

    luabridge::setGlobal(L, RESOURCES_PATH.string(), "resources_directory");

    luabridge::getGlobalNamespace(L)
        .beginClass<glm::vec2>("vec2")
        .addProperty("x", &glm::vec2::x)
        .addProperty("y", &glm::vec2::y)
        .endClass();

    luabridge::getGlobalNamespace(L)
        .beginClass<Actor>("Actor")
        .addFunction("GetName", &Actor::getName)
        .addFunction("GetID", &Actor::getId)
        .addFunction("GetComponentByKey", &Actor::getComponentByKey)
        .addFunction("GetComponent", &Actor::getComponent)
        .addFunction("GetComponents", &Actor::getComponents)
        .addFunction("AddComponent", &Actor::addComponent)
        .addFunction("RemoveComponent", &Actor::removeComponent)
        .endClass();

    luabridge::getGlobalNamespace(L)
        .beginNamespace("Actor")
        .addFunction("Find", &SceneDB::findActor)
        .addFunction("FindAll", &SceneDB::findAllActors)
        .addFunction("Instantiate", &SceneDB::instantiateActor)
        .addFunction("Destroy", &SceneDB::destroyActor)
        .endNamespace();

    luabridge::getGlobalNamespace(L)
        .beginNamespace("Debug")
        .addFunction("Log", &Debug::log)
        .endNamespace();

    luabridge::getGlobalNamespace(L)
        .beginNamespace("Application")
        .addFunction("Quit", &Application::quit)
        .addFunction("Sleep", &Application::sleep)
        .addFunction("GetFrame", &Application::getFrame)
        .addFunction("OpenURL", &Application::openUrl)
        .endNamespace();

    luabridge::getGlobalNamespace(L)
        .beginNamespace("Input")
        .addFunction("GetKey", &Input::getKey)
        .addFunction("GetKeyDown", &Input::getKeyDown)
        .addFunction("GetKeyUp", &Input::getKeyUp)
        .addFunction("GetMousePosition", &Input::getMousePosition)
        .addFunction("GetMouseButton", &Input::getMouseButton)
        .addFunction("GetMouseButtonDown", &Input::getMouseButtonDown)
        .addFunction("GetMouseButtonUp", &Input::getMouseButtonUp)
        .addFunction("GetMouseScrollDelta", &Input::getMouseScrollDelta)
        .addFunction("HideCursor", &Input::hideCursor)
        .addFunction("ShowCursor", &Input::showCursor)
        .endNamespace();

    luabridge::getGlobalNamespace(L)
        .beginNamespace("Text")
        .addFunction("Draw", Renderer::drawText)
        .endNamespace();

    luabridge::getGlobalNamespace(L)
        .beginNamespace("Audio")
        .addFunction("Play", Audio::play)
        .addFunction("Halt", Audio::halt)
        .addFunction("SetVolume", Audio::setVolume)
        .endNamespace();

    luabridge::getGlobalNamespace(L)
        .beginNamespace("Image")
        .addFunction("Draw", Renderer::draw)
        .addFunction("DrawEx", Renderer::drawEx)
        .addFunction("DrawUI", Renderer::drawUi)
        .addFunction("DrawUIEx", Renderer::drawUiEx)
        .addFunction("DrawPixel", Renderer::drawPixel)
        .endNamespace();

    luabridge::getGlobalNamespace(L)
        .beginNamespace("Camera")
        .addFunction("SetPosition", Renderer::setPosition)
        .addFunction("GetPositionX", Renderer::getPositionX)
        .addFunction("GetPositionY", Renderer::getPositionY)
        .addFunction("SetZoom", Renderer::setZoom)
        .addFunction("GetZoom", Renderer::getZoom)
        .endNamespace();

    luabridge::getGlobalNamespace(L)
        .beginNamespace("Scene")
        .addFunction("Load", SceneDB::load)
        .addFunction("GetCurrent", SceneDB::getCurrentScene)
        .addFunction("DontDestroy", SceneDB::dontDestroy)
        .endNamespace();

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

    luabridge::getGlobalNamespace(L)
        .beginNamespace("Event")
        .addFunction("Publish", EventManager::publish)
        .addFunction("Subscribe", EventManager::subscribe)
        .addFunction("Unsubscribe", EventManager::unsubscribe)
        .endNamespace();

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
