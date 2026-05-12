#include "audio.h"

#include <lua/lua.hpp>

#include <LuaBridge/LuaBridge.h>

void Audio::registerLuaBindings(lua_State *L) {
    luabridge::getGlobalNamespace(L)
        .beginNamespace("Audio")
        .addFunction("Play", Audio::play)
        .addFunction("Halt", Audio::halt)
        .addFunction("SetVolume", Audio::setVolume)
        .endNamespace();
}
