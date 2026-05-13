#include "actor.h"

#include <lua/lua.hpp>

#include <LuaBridge/LuaBridge.h>

void Actor::createLuaBindings(lua_State *L) {
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
}
