#include "engine.h"

#include <lua/lua.hpp>

#include <LuaBridge/LuaBridge.h>

void Engine::registerLuaBindings() {
    luabridge::getGlobalNamespace(L)
        .beginNamespace("Application")
        .addFunction("Quit", []() { exit(0); })
        .addFunction("Sleep", &Engine::sleep)
        .addFunction("GetFrame", &Engine::getFrameNumber)
        .addFunction("OpenURL", &Engine::openUrl)
        .endNamespace();
}
