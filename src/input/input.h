#pragma once

#include <glm/ext/vector_float2.hpp>

#include <SDL_events.h>
#include <SDL_gamecontroller.h>
#include <SDL_joystick.h>
#include <SDL_scancode.h>
#include <SDL_stdinc.h>

#include <array>
#include <cstdint>
#include <string>
#include <unordered_map>

struct lua_State;

enum KeyState : uint8_t { UP, JUST_DOWN, DOWN, JUST_UP };

struct ControllerState {
    bool connected = false;
    SDL_GameController* handle = nullptr;
    std::array<KeyState, SDL_CONTROLLER_BUTTON_MAX> buttons{};
    std::array<float, SDL_CONTROLLER_AXIS_MAX> axes{};
};

class InputState {
public:
    explicit InputState() = default;

    // Runs every frame
    static void resetFrame();
    static void handleEvent(SDL_Event &e);

    // View keyboard
    static bool getKey(const std::string &keycode);
    static bool getKeyDown(const std::string &keycode);
    static bool getKeyUp(const std::string &keycode);

    // View mouse
    static bool getMouseButton(Uint8 b) {
        return mouseKeyStates[b] == JUST_DOWN || mouseKeyStates[b] == DOWN;
    }

    static bool getMouseButtonDown(Uint8 b) {
        return mouseKeyStates[b] == JUST_DOWN;
    }

    static bool getMouseButtonUp(Uint8 b) { return mouseKeyStates[b] == JUST_UP; }
    static glm::vec2 getMousePosition() { return mousePosition; };
    static float getMouseScrollDelta() { return scrollDelta; };

    // Cursor
    static void hideCursor() { SDL_ShowCursor(SDL_DISABLE); }
    static void showCursor() { SDL_ShowCursor(SDL_ENABLE); }

    // Controller features
    static void initializeController();

    // Lua
    static void registerLuaBindings(lua_State *L);

private:
    static inline std::array<KeyState, SDL_NUM_SCANCODES> keyboardKeyStates;
    static inline std::array<KeyState, 4> mouseKeyStates;
    static inline glm::vec2 mousePosition;
    static inline float scrollDelta = 0.0F;
    static inline std::array<ControllerState, 4> controllerStates;
    static inline std::unordered_map<SDL_JoystickID, int> instanceToSlot;
};
