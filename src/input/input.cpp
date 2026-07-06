#include "input.h"

#include <SDL_events.h>
#include <SDL_gamecontroller.h>

#include <cstdint>

namespace {
inline bool playerIsInvalid(int player) { return (player < 0 || player >= 8); }

inline void setKeyToHold(KeyState &k) {
    // JUST_DOWN and JUST_UP are enums 1 and 3, respectively
    const uint8_t justOn = (k & 1);
    k = static_cast<KeyState>((k + justOn) & 0b11);
}
} // namespace

void InputState::resetFrame() {
    for (KeyState &k : keyboard.buttons) {
        setKeyToHold(k);
    }
    for (KeyState &k : mouse.buttons) {
        setKeyToHold(k);
    }
    for (ControllerState &controller : controllers) {
        for (KeyState &k : controller.buttons) {
            setKeyToHold(k);
        }
    }
}

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
    const KeyState k = keyboard.getButton(keycode);
    return k == JUST_DOWN || k == DOWN;
}
bool InputState::getKeyDown(const char *keycode) {
    const KeyState k = keyboard.getButton(keycode);
    return k == JUST_DOWN;
}
bool InputState::getKeyUp(const char *keycode) {
    const KeyState k = keyboard.getButton(keycode);
    return k == JUST_UP;
}

bool InputState::getMouseButton(uint8_t b) {
    const KeyState k = mouse.getButton(b);
    return k == JUST_DOWN || k == DOWN;
}
bool InputState::getMouseButtonDown(uint8_t b) {
    const KeyState k = mouse.getButton(b);
    return k == JUST_DOWN;
}
bool InputState::getMouseButtonUp(uint8_t b) {
    const KeyState k = mouse.getButton(b);
    return k == JUST_UP;
}

glm::vec2 InputState::getMousePosition() { return mouse.position; }
float InputState::getMouseScrollDelta() { return mouse.scrollDelta; }

bool InputState::getControllerKey(int player, const char *keycode) {
    KeyState k = controllers[player - 1].getButton(keycode);
    return k == JUST_DOWN || k == DOWN;
}
bool InputState::getControllerKeyDown(int player, const char *keycode) {
    KeyState k = controllers[player - 1].getButton(keycode);
    return k == JUST_DOWN;
}
bool InputState::getControllerKeyUp(int player, const char *keycode) {
    KeyState k = controllers[player - 1].getButton(keycode);
    return k == JUST_UP;
}
