#pragma once

#include "lua/lua.hpp"

#include "LuaBridge/LuaBridge.h"

#include <rapidjson/document.h>

struct Actor;

struct ComponentManager {
    explicit ComponentManager();

    static luabridge::LuaRef
    createComponent(const std::string &type,
                    const rapidjson::Value &propertiesJson, Actor *actor,
                    const std::string &key);

    static luabridge::LuaRef getGlobalRef();
    static inline lua_State *L;
};
