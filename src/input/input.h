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

static constexpr int NUM_MAX_PLAYERS = 8;

struct KeyboardState {
    std::array<KeyState, SDL_NUM_SCANCODES> buttons{};

    [[nodiscard]] KeyState getButton(const char *keycode) const;
    void setButton(SDL_Scancode button, KeyState keyState);

    void resetFrame();
};

struct MouseState {
    std::array<KeyState, 4> buttons{}; // Null, left, middle, right
    glm::vec2 position{0.0F, 0.0F};
    float scrollDelta = 0.0F;

    [[nodiscard]] KeyState getButton(uint8_t button) const;
    void setButton(uint8_t button, KeyState keyState);
    [[nodiscard]] glm::vec2 getMousePosition() const;
    void setMousePosition(glm::vec2);
    [[nodiscard]] float getScrollDelta() const;
    void addScrollDelta(float delta);

    void resetFrame();
};

struct ControllerState {
    std::array<KeyState, SDL_CONTROLLER_BUTTON_MAX> buttons{};
    std::array<float, SDL_CONTROLLER_AXIS_MAX> axes{};

    [[nodiscard]] KeyState getButton(const char *keycode) const;
    void setButton(SDL_GameControllerButton button, KeyState keyState);
    [[nodiscard]] float getAxis(const char *keycode) const;
    void setAxis(SDL_GameControllerAxis axis, float value);

    void resetFrame();
};

class InputState {
public:
    explicit InputState() = default;

    static inline KeyboardState keyboard;
    static inline MouseState mouse;
    static inline std::array<ControllerState, NUM_MAX_PLAYERS> controllers;

    // Runs every frame
    static void resetFrame();
    static void handleEvent(SDL_Event &e);

    // View keyboard
    static bool getKey(const char *keycode);
    static bool getKeyDown(const char *keycode);
    static bool getKeyUp(const char *keycode);

    // View mouse
    static bool getMouseButton(uint8_t b);
    static bool getMouseButtonDown(uint8_t b);
    static bool getMouseButtonUp(uint8_t b);

    static glm::vec2 getMousePosition();
    static float getMouseScrollDelta();

    static void hideCursor() { SDL_ShowCursor(SDL_DISABLE); }
    static void showCursor() { SDL_ShowCursor(SDL_ENABLE); }

    // View controller
    static bool getControllerKey(const char *keycode, int player);
    static bool getControllerKeyUp(const char *keycode, int player);
    static bool getControllerKeyDown(const char *keycode, int player);
    static int getNumControllers();

    // Lua
    static void registerLuaBindings(lua_State *L);

private:
    // Controller states
    static inline int numControllersOpen = 0;
};
