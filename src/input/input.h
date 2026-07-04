#pragma once

#include <glm/ext/vector_float2.hpp>

#include <SDL_events.h>
#include <SDL_gamecontroller.h>
#include <SDL_joystick.h>
#include <SDL_scancode.h>
#include <SDL_stdinc.h>

#include <array>
#include <cstdint>

struct lua_State;

enum KeyState : uint8_t { UP, JUST_DOWN, DOWN, JUST_UP };

struct ControllerState {
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
    static bool getKey(const char *keycode);
    static bool getKeyDown(const char *keycode);
    static bool getKeyUp(const char *keycode);

    // View mouse
    static bool getMouseButton(Uint8 b) {
        return mouseKeyStates[b] == JUST_DOWN || mouseKeyStates[b] == DOWN;
    }

    static bool getMouseButtonDown(Uint8 b) {
        return mouseKeyStates[b] == JUST_DOWN;
    }

    static bool getMouseButtonUp(Uint8 b) {
        return mouseKeyStates[b] == JUST_UP;
    }
    static glm::vec2 getMousePosition() { return mousePosition; };
    static float getMouseScrollDelta() { return scrollDelta; };

    // View controller
    static bool getControllerKey(int player, const char *keycode);
    static bool getControllerKeyUp(int player, const char *keycode);
    static bool getControllerKeyDown(int player, const char *keycode);

    // Cursor
    static void hideCursor() { SDL_ShowCursor(SDL_DISABLE); }
    static void showCursor() { SDL_ShowCursor(SDL_ENABLE); }

    // Lua
    static void registerLuaBindings(lua_State *L);

private:
    // Keyboard and mouse states
    static inline std::array<KeyState, SDL_NUM_SCANCODES> keyboardKeyStates;
    static inline std::array<KeyState, 4> mouseKeyStates;
    static inline glm::vec2 mousePosition;
    static inline float scrollDelta = 0.0F;

    // Controller states
    static inline uint8_t numControllersOpen = 0;
    static constexpr int NUM_MAX_PLAYERS = 8;
    static inline std::array<ControllerState, NUM_MAX_PLAYERS> controllerStates;

    // Helper functions
    static inline KeyState getKeyboardKeyState(const char *keycode);
    static inline KeyState getControllerKeyState(int player, // starts with 1
                                                 const char *keycode);
};
