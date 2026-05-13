#include "input.h"

#include <lua/lua.hpp>

#include <LuaBridge/LuaBridge.h>

void Input::registerLuaBindings(lua_State *L) {
    luabridge::getGlobalNamespace(L)
        .beginNamespace("Input")
        .addFunction("GetKey", &Input::getKey)
        .addFunction("GetKeyDown", &Input::getKeyDown)
        .addFunction("GetKeyUp", &Input::getKeyUp)
        .addFunction("GetMousePosition", &Input::getMousePosition)
        .addFunction("GetMouseButton", &Input::getMouseButton)
        .addFunction("GetMouseButtonDown", &Input::getMouseButtonDown)
        .addFunction("GetMouseButtonUp", &Input::getMouseButtonUp)
        .addFunction("GetMouseScrollDelta", &Input::getMouseScrollDelta)
        .addFunction("HideCursor", &Input::hideCursor)
        .addFunction("ShowCursor", &Input::showCursor)
        .endNamespace();
}
