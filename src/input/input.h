#pragma once

#include <SDL_events.h>
#include <SDL_gamecontroller.h>
#include <SDL_scancode.h>
#include <SDL_stdinc.h>
#include "glm/ext/vector_float2.hpp"

#include <array>
#include <cstdint>
#include <string>

class Input {
public:
    explicit Input() = default;
    enum KeyState : uint8_t { UP, JUST_DOWN, DOWN, JUST_UP };

    // Runs every frame
    static void resetFrame();
    static void handleEvent(SDL_Event &e);

    // View keyboard
    static bool getKey(const std::string &keycode);
    static bool getKeyDown(const std::string &keycode);
    static bool getKeyUp(const std::string &keycode);

    // View mouse
    static bool getMouseButton(Uint8 b) {
        return mouseKeys[b] == JUST_DOWN || mouseKeys[b] == DOWN;
    }

    static bool getMouseButtonDown(Uint8 b) {
        return mouseKeys[b] == JUST_DOWN;
    }
    static bool getMouseButtonUp(Uint8 b) { return mouseKeys[b] == JUST_UP; }
    static glm::vec2 getMousePosition() { return mousePosition; };
    static float getMouseScrollDelta() { return scrollDelta; };

    // Cursor
    static void hideCursor() { SDL_ShowCursor(SDL_DISABLE); }
    static void showCursor() { SDL_ShowCursor(SDL_ENABLE); }

    // Controller features
    static void initializeController();

private:
    static inline std::array<KeyState, SDL_NUM_SCANCODES> keys;
    static inline std::array<KeyState, 4> mouseKeys;
    static inline glm::vec2 mousePosition;
    static inline float scrollDelta = 0.0F;
    static inline float rightStickY = 0.0F;

    static inline SDL_GameController *controller = nullptr;
};
