#include "input.h"

#include <lua/lua.hpp>

#include <LuaBridge/LuaBridge.h>

void InputState::registerLuaBindings(lua_State *L) {
    luabridge::getGlobalNamespace(L)
        .beginNamespace("Input")

        // Keyboard
        .addFunction("GetKey", &InputState::getKey)
        .addFunction("GetKeyDown", &InputState::getKeyDown)
        .addFunction("GetKeyUp", &InputState::getKeyUp)

        // Mouse
        .addFunction("GetMousePosition", &InputState::getMousePosition)
        .addFunction("GetMouseButton", &InputState::getMouseButton)
        .addFunction("GetMouseButtonDown", &InputState::getMouseButtonDown)
        .addFunction("GetMouseButtonUp", &InputState::getMouseButtonUp)
        .addFunction("GetMouseScrollDelta", &InputState::getMouseScrollDelta)
        .addFunction("HideCursor", &InputState::hideCursor)
        .addFunction("ShowCursor", &InputState::showCursor)

        // Controller
        .addFunction("GetControllerKey", &InputState::getControllerKey)
        .addFunction("GetControllerKeyDown", &InputState::getControllerKeyDown)
        .addFunction("GetControllerKeyUp", &InputState::getControllerKeyUp)

        .endNamespace();
}
