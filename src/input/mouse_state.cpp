#include "input.h"

KeyState MouseState::getButton(uint8_t button) const { return buttons[button]; }

void MouseState::setButton(uint8_t button, KeyState keyState) {
    buttons[static_cast<size_t>(button)] = keyState;
}

glm::vec2 MouseState::getMousePosition() const { return position; }

void MouseState::setMousePosition(glm::vec2 p) { position = p; }

float MouseState::getScrollDelta() const { return scrollDelta; }

void MouseState::addScrollDelta(float delta) { scrollDelta += delta; }

void MouseState::resetFrame() {
    for (KeyState &k : buttons) {
        const uint8_t justOn = (k & 1);
        k = static_cast<KeyState>((k + justOn) & 0b11);
    }
    scrollDelta = 0;
}
