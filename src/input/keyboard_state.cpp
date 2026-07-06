#include "input.h"

#include "keyboard_scancode_map.h"

bool KeyboardState::getKey(const char* keycode) const {
    const KeyState k = getButton(keycode);
    return k == JUST_DOWN || k == DOWN;
}

bool KeyboardState::getKeyDown(const char* keycode) const {
    return getButton(keycode) == JUST_DOWN;
}

bool KeyboardState::getKeyUp(const char* keycode) const {
    return getButton(keycode) == JUST_UP;
}

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
