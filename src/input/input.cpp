#include "input.h"

#include "keyboard_scancode_map.h"

#include <SDL_events.h>
#include <SDL_gamecontroller.h>

#include <cstdint>
#include <unordered_map>

namespace {
inline bool playerIsInvalid(int player) { return (player < 0 || player >= 8); }

inline void setKeyToHold(KeyState &k) {
    // JUST_DOWN and JUST_UP are enums 1 and 3, respectively
    const uint8_t justOn = (k & 1);
    k = static_cast<KeyState>((k + justOn) & 0b11);
}
} // namespace

KeyState InputState::getKeyboardKeyState(const char *keycode) {
    const auto it = keycodeToScancode.find(keycode);
    if (it == keycodeToScancode.end()) {
        return KeyState::UP;
    }
    return keyboardKeyStates[it->second];
}

KeyState InputState::getControllerKeyState(int player, const char *keycode) {
    player -= 1; // Lua API 1-indexes everything, player 1 will index at 0
    if (playerIsInvalid(player)) {
        return UP;
    }
    const SDL_GameControllerButton button =
        SDL_GameControllerGetButtonFromString(keycode);
    if (button == SDL_CONTROLLER_BUTTON_INVALID) {
        return UP;
    }
    return controllerStates[static_cast<size_t>(player)]
        .buttons[static_cast<size_t>(button)];
}

void InputState::resetFrame() {
    for (KeyState &k : keyboardKeyStates) {
        setKeyToHold(k);
    }
    for (KeyState &k : mouseKeyStates) {
        setKeyToHold(k);
    }
    for (ControllerState &state : controllerStates) {
        for (KeyState &b : state.buttons) {
            setKeyToHold(b);
        }
    }
}

void InputState::handleEvent(SDL_Event &e) {
    switch (e.type) {
    case SDL_KEYDOWN:
        keyboardKeyStates[e.key.keysym.scancode] = JUST_DOWN;
        break;
    case SDL_KEYUP:
        keyboardKeyStates[e.key.keysym.scancode] = JUST_UP;
        break;
    case SDL_MOUSEBUTTONDOWN:
        mouseKeyStates[e.button.button] = JUST_DOWN;
        break;
    case SDL_MOUSEBUTTONUP:
        mouseKeyStates[e.button.button] = JUST_UP;
        break;
    case SDL_MOUSEMOTION:
        mousePosition = {e.motion.x, e.motion.y};
        break;
    case SDL_MOUSEWHEEL:
        scrollDelta += e.wheel.preciseY;
        break;
    case SDL_CONTROLLERAXISMOTION: {
        const int player = SDL_GameControllerGetPlayerIndex(
            SDL_GameControllerFromInstanceID(e.caxis.which));
        if (playerIsInvalid(player)) {
            break;
        }
        controllerStates[static_cast<size_t>(player)].axes[e.caxis.axis] =
            static_cast<float>(e.caxis.value) / static_cast<float>(INT16_MAX);
        break;
    }
    case SDL_CONTROLLERBUTTONDOWN: {
        const int player = SDL_GameControllerGetPlayerIndex(
            SDL_GameControllerFromInstanceID(e.cbutton.which));
        if (playerIsInvalid(player)) {
            break;
        }
        controllerStates[static_cast<size_t>(player)]
            .buttons[e.cbutton.button] = JUST_DOWN;
        break;
    };
    case SDL_CONTROLLERBUTTONUP: {
        const int player = SDL_GameControllerGetPlayerIndex(
            SDL_GameControllerFromInstanceID(e.cbutton.which));
        if (playerIsInvalid(player)) {
            break;
        }
        controllerStates[static_cast<size_t>(player)]
            .buttons[e.cbutton.button] = JUST_UP;
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

// View keyboard
bool InputState::getKey(const char *keycode) {
    const KeyState k = getKeyboardKeyState(keycode);
    return k == JUST_DOWN || k == DOWN;
}
bool InputState::getKeyDown(const char *keycode) {
    const KeyState k = getKeyboardKeyState(keycode);
    return k == JUST_DOWN;
}
bool InputState::getKeyUp(const char *keycode) {
    const KeyState k = getKeyboardKeyState(keycode);
    return k == JUST_UP;
}
bool InputState::getControllerKey(int player, const char *keycode) {
    KeyState k = getControllerKeyState(player, keycode);
    return k == JUST_DOWN || k == DOWN;
}
bool InputState::getControllerKeyDown(int player, const char *keycode) {
    KeyState k = getControllerKeyState(player, keycode);
    return k == JUST_DOWN;
}
bool InputState::getControllerKeyUp(int player, const char *keycode) {
    KeyState k = getControllerKeyState(player, keycode);
    return k == JUST_UP;
}
