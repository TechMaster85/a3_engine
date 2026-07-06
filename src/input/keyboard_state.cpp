#include "input.h"

#include "keyboard_scancode_map.h"

KeyState KeyboardState::getButton(const char* keycode) const {
    const auto it = keycodeToScancode.find(keycode);
    if (it == keycodeToScancode.end()) {
        return KeyState::UP;
    }
    return buttons[it->second];
}

void KeyboardState::setButton(SDL_Scancode button, KeyState keyState) {
    buttons[button] = keyState;
}

void KeyboardState::resetFrame() {
    for (KeyState &k : buttons) {
        const uint8_t justOn = (k & 1);
        k = static_cast<KeyState>((k + justOn) & 0b11);
    }
}
