#include "input.h"

#include <SDL_events.h>
#include <SDL_gamecontroller.h>

#include <cassert>
#include <cstdint>

namespace {
// player is a 0-indexed slot into InputState::controllers
inline bool playerIsInvalid(int player) {
    return (player < 0 || player >= NUM_MAX_PLAYERS);
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
        // e.cdevice.which is a device index here, not an instance ID.
        // SDL auto-assigns a free player index on add (SDL_joystick.c's
        // SDL_FindFreePlayerIndex), so no manual bookkeeping is needed.
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

bool InputState::getControllerKey(const char *keycode, int player) {
    const int slot = player - 1;
    return !playerIsInvalid(slot) &&
           isDown(controllers[static_cast<size_t>(slot)].getButton(keycode));
}
bool InputState::getControllerKeyDown(const char *keycode, int player) {
    const int slot = player - 1;
    return !playerIsInvalid(slot) && isJustDown(controllers[static_cast<size_t>(slot)]
                                                     .getButton(keycode));
}
bool InputState::getControllerKeyUp(const char *keycode, int player) {
    const int slot = player - 1;
    return !playerIsInvalid(slot) &&
           isJustUp(controllers[static_cast<size_t>(slot)].getButton(keycode));
}
int InputState::getNumControllers() {
    return numControllersOpen;
}

void InputState::resetFrame() {
    keyboard.resetFrame();
    mouse.resetFrame();
    for (ControllerState &controller : controllers) {
        controller.resetFrame();
    }
}
