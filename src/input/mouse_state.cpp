#include "input.h"


bool MouseState::getKey(uint8_t button) const {
    const KeyState k = getButton(button);
    return k == JUST_DOWN || k == DOWN;
}

bool MouseState::getKeyDown(uint8_t button) const {
    return getButton(button) == JUST_DOWN;
}

bool MouseState::getKeyUp(uint8_t button) const {
    return getButton(button) == JUST_UP;
}

KeyState MouseState::getButton(uint8_t button) const {
    return buttons[button];
}

void MouseState::setButton(SDL_Scancode button, KeyState keyState) {
    buttons[button] = keyState;
}

glm::vec2 MouseState::getMousePosition() const {
    return position;
}

void MouseState::setMousePosition(glm::vec2 p) {
    position = p;
}

float MouseState::getScrollDelta() const {
    return scrollDelta;
}

void MouseState::addScrollDelta(float delta) {
    scrollDelta += delta;
}

void MouseState::resetFrame() {
    for (KeyState &k : buttons) {
        const uint8_t justOn = (k & 1);
        k = static_cast<KeyState>((k + justOn) & 0b11);
    }
}
