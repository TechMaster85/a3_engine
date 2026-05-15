#pragma once

#include <string>

struct lua_State;

namespace Debug {
void log(const std::string &msg);
void error(const std::string &msg);
void runtimeError(const std::string &actor, const std::string &msg);
void registerLuaBindings(lua_State *L);
} // namespace Debug
