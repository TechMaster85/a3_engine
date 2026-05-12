#pragma once

struct lua_State;

namespace LuaBindings {
    void registerAll(lua_State *L);
};
