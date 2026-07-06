#include "input.h"

#include <SDL_events.h>
#include <SDL_gamecontroller.h>

#include <cassert>
#include <cstdint>

namespace {
inline bool playerIsInvalid(int player) {
    return (player < 1 || player > NUM_MAX_PLAYERS);
}

inline bool isDown(KeyState k) { return k == JUST_DOWN || k == DOWN; }
inline bool isJustDown(KeyState k) { return k == JUST_DOWN; }
inline bool isJustUp(KeyState k) { return k == JUST_UP; }
} // namespace

void InputState::handleEvent(SDL_Event &e) {
    switch (e.type) {
    case SDL_KEYDOWN:
        keyboard.buttons[e.key.keysym.scancode] = JUST_DOWN;
        break;
    case SDL_KEYUP:
        keyboard.buttons[e.key.keysym.scancode] = JUST_UP;
        break;
    case SDL_MOUSEBUTTONDOWN:
        mouse.buttons[e.button.button] = JUST_DOWN;
        break;
    case SDL_MOUSEBUTTONUP:
        mouse.buttons[e.button.button] = JUST_UP;
        break;
    case SDL_MOUSEMOTION:
        mouse.position = {e.motion.x, e.motion.y};
        break;
    case SDL_MOUSEWHEEL:
        mouse.scrollDelta += e.wheel.preciseY;
        break;
    case SDL_CONTROLLERAXISMOTION: {
        const int player = SDL_GameControllerGetPlayerIndex(
            SDL_GameControllerFromInstanceID(e.caxis.which));
        if (playerIsInvalid(player)) {
            break;
        }
        controllers[static_cast<size_t>(player)].axes[e.caxis.axis] =
            static_cast<float>(e.caxis.value) / static_cast<float>(INT16_MAX);
        break;
    }
    case SDL_CONTROLLERBUTTONDOWN: {
        const int player = SDL_GameControllerGetPlayerIndex(
            SDL_GameControllerFromInstanceID(e.cbutton.which));
        if (playerIsInvalid(player)) {
            break;
        }
        controllers[static_cast<size_t>(player)].buttons[e.cbutton.button] =
            JUST_DOWN;
        break;
    };
    case SDL_CONTROLLERBUTTONUP: {
        const int player = SDL_GameControllerGetPlayerIndex(
            SDL_GameControllerFromInstanceID(e.cbutton.which));
        if (playerIsInvalid(player)) {
            break;
        }
        controllers[static_cast<size_t>(player)].buttons[e.cbutton.button] =
            JUST_UP;
        break;
    }
    case SDL_CONTROLLERDEVICEADDED: {
        SDL_GameController *controller =
            SDL_GameControllerOpen(e.cdevice.which);
        if (controller == nullptr) {
            break;
        }
        ++numControllersOpen;
        break;
    }
    case SDL_CONTROLLERDEVICEREMOVED: {
        SDL_GameController *controller =
            SDL_GameControllerFromInstanceID(e.cdevice.which);
        SDL_GameControllerClose(controller);
        --numControllersOpen;
        break;
    }
    default:
        break;
    }
}

// View
bool InputState::getKey(const char *keycode) {
    return isDown(keyboard.getButton(keycode));
}
bool InputState::getKeyDown(const char *keycode) {
    return isJustDown(keyboard.getButton(keycode));
}
bool InputState::getKeyUp(const char *keycode) {
    return isJustUp(keyboard.getButton(keycode));
}

bool InputState::getMouseButton(uint8_t b) {
    return isDown(mouse.getButton(b));
}
bool InputState::getMouseButtonDown(uint8_t b) {
    return isJustDown(mouse.getButton(b));
}
bool InputState::getMouseButtonUp(uint8_t b) {
    return isJustUp(mouse.getButton(b));
}

glm::vec2 InputState::getMousePosition() { return mouse.position; }
float InputState::getMouseScrollDelta() { return mouse.scrollDelta; }

bool InputState::getControllerKey(int player, const char *keycode) {
    return !playerIsInvalid(player) && isDown(
        controllers[static_cast<size_t>(player - 1)].getButton(keycode));
}
bool InputState::getControllerKeyDown(int player, const char *keycode) {
    return !playerIsInvalid(player) &&
           isJustDown(
               controllers[static_cast<size_t>(player - 1)].getButton(keycode));
}
bool InputState::getControllerKeyUp(int player, const char *keycode) {
    return !playerIsInvalid(player) && isJustUp(
        controllers[static_cast<size_t>(player - 1)].getButton(keycode));
}

void InputState::resetFrame() {
    keyboard.resetFrame();
    mouse.resetFrame();
    for (ControllerState &controller : controllers) {
        controller.resetFrame();
    }
}
