#include "scenedb.h"

#include <lua/lua.hpp>

#include <LuaBridge/LuaBridge.h>

void SceneDB::registerLuaBindings(lua_State *L) {
    luabridge::getGlobalNamespace(L)
        .beginClass<Actor>("Actor")
        .addStaticFunction("Find", &SceneDB::findActor)
        .addStaticFunction("FindAll", &SceneDB::findAllActors)
        .addStaticFunction("Instantiate", &SceneDB::instantiateActor)
        .addStaticFunction("Destroy", &SceneDB::destroyActor)
        .endClass();

    luabridge::getGlobalNamespace(L)
        .beginNamespace("Scene")
        .addFunction("Load", SceneDB::load)
        .addFunction("GetCurrent", SceneDB::getCurrentScene)
        .addFunction("DontDestroy", SceneDB::dontDestroy)
        .endNamespace();
}
