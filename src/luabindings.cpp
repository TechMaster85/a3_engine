#include "luabindings.h"

#include "audio/audio.h"
#include "core/debug.h"
#include "core/fileutil.h"
#include "engine.h"
#include "events/eventmanager.h"
#include "input/input.h"
#include "particles/particlesystem.h"
#include "rendering/renderer.h"
#include "physics/rigidbody.h"
#include "scene/actor.h"
#include "scene/scenedb.h"

#include <LuaBridge/LuaBridge.h>

void LuaBindings::registerAll(lua_State *L) {
    luabridge::setGlobal(L, FileUtil::RESOURCES_PATH.string(),
                         "resources_directory");

    luabridge::getGlobalNamespace(L)
        .beginClass<glm::vec2>("vec2")
        .addProperty("x", &glm::vec2::x)
        .addProperty("y", &glm::vec2::y)
        .endClass();

    Engine::registerLuaBindings();  // L is part of engine already
    Actor::registerLuaBindings(L);
    Input::registerLuaBindings(L);
    Debug::registerLuaBindings(L);
    SceneDB::registerLuaBindings(L);
    Audio::registerLuaBindings(L);
    Renderer::registerLuaBindings(L);
    Rigidbody::registerLuaBindings(L);
    ParticleSystem::registerLuaBindings(L);
    EventManager::registerLuaBindings(L);
}
