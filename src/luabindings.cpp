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

    Engine::registerLuaBindings();  // L is part of engine already
    Actor::createLuaBindings(L);
    SceneDB::createLuaBindings(L);
    Input::registerLuaBindings(L);
    Audio::registerLuaBindings(L);
    Renderer::registerLuaBindings(L);
    Rigidbody::registerLuaBindings(L);
    ParticleSystem::registerLuaBindings(L);
    EventManager::registerLuaBindings(L);

}
