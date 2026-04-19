#pragma once

#include "lua/lua.hpp"

#include "LuaBridge/LuaBridge.h"

struct LuaBindings {
    explicit LuaBindings(lua_State *L);

    static lua_State *getState();

private:
    static lua_State *state;
};
