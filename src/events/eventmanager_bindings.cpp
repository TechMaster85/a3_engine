#include "eventmanager.h"

#include <lua/lua.hpp>

#include <LuaBridge/LuaBridge.h>

void EventManager::registerLuaBindings(lua_State *L) {
    luabridge::getGlobalNamespace(L)
        .beginNamespace("Event")
        .addFunction("Publish", EventManager::publish)
        .addFunction("Subscribe", EventManager::subscribe)
        .addFunction("Unsubscribe", EventManager::unsubscribe)
        .endNamespace();
}
