#include "luabindings.h"

#include "audio/audio.h"
#include "core/fileutil.h"
#include "engine.h"
#include "events/eventmanager.h"
#include "input/input.h"
#include "particles/particlesystem.h"
#include "rendering/renderer.h"
#include "physics/rigidbody.h"
#include "scene/actor.h"
#include "scene/scenedb.h"

#include <lua/lua.hpp>

#include <LuaBridge/LuaBridge.h>

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
    Rigidbody::registerLuaBindings(L);

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
