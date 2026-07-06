#include "input.h"
#include <SDL_gamecontroller.h>

KeyState ControllerState::getButton(const char *keycode) const {
    const SDL_GameControllerButton button =
        SDL_GameControllerGetButtonFromString(keycode);
    return buttons[static_cast<size_t>(button)];
}

void ControllerState::setButton(SDL_GameControllerButton button,
                                KeyState keyState) {
    buttons[static_cast<size_t>(button)] = keyState;
}

float ControllerState::getAxis(const char *keycode) const {
    const SDL_GameControllerAxis axis =
        SDL_GameControllerGetAxisFromString(keycode);
    return axes[static_cast<size_t>(axis)];
}

void ControllerState::setAxis(SDL_GameControllerAxis axis, float value) {
    axes[static_cast<size_t>(axis)] = value;
}

void ControllerState::resetFrame() {
    for (KeyState &k : buttons) {
        const uint8_t justOn = (k & 1);
        k = static_cast<KeyState>((k + justOn) & 0b11);
    }
}
