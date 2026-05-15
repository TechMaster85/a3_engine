#include "debug.h"

#include <lua/lua.hpp>

#include <LuaBridge/LuaBridge.h>

#include <iostream>

namespace {
void luaLog(const luabridge::LuaRef &message) {
    if (message.isNil()) {
        std::cout << '\n';
    } else {
        std::cout << message.tostring() << '\n';
    }
    std::cout << std::flush;
}
} // namespace

namespace Debug {
void log(const std::string &msg) {
    std::cout << msg << '\n' << std::flush;
}

void error(const std::string &msg) {
    std::cout << "error: " << msg << '\n' << std::flush;
}

void runtimeError(const std::string &actor, const std::string &msg) {
    std::cout << "\033[31m" << actor << " : " << msg << "\033[0m" << '\n' << std::flush;
}

void registerLuaBindings(lua_State *L) {
    luabridge::getGlobalNamespace(L)
        .beginNamespace("Debug")
        .addFunction("Log", &luaLog)
        .endNamespace();
}
} // namespace Debug
