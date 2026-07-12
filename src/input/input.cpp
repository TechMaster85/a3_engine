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

int InputState::findPlayerSlot(SDL_JoystickID instanceId) {
    for (int i = 0; i < NUM_MAX_PLAYERS; ++i) {
        if (slotInstanceId[static_cast<size_t>(i)] == instanceId) {
            return i;
        }
    }
    return -1;
}

int InputState::allocatePlayerSlot(SDL_JoystickID instanceId) {
    const int slot = findPlayerSlot(-1); // first empty slot
    if (slot != -1) {
        slotInstanceId[static_cast<size_t>(slot)] = instanceId;
    }
    return slot;
}

void InputState::freePlayerSlot(SDL_JoystickID instanceId) {
    const int slot = findPlayerSlot(instanceId);
    if (slot != -1) {
        slotInstanceId[static_cast<size_t>(slot)] = -1;
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
        const int player = findPlayerSlot(e.caxis.which);
        if (playerIsInvalid(player)) {
            break;
        }
        controllers[static_cast<size_t>(player)].axes[e.caxis.axis] =
            static_cast<float>(e.caxis.value) / static_cast<float>(INT16_MAX);
        break;
    }
    case SDL_CONTROLLERBUTTONDOWN: {
        const int player = findPlayerSlot(e.cbutton.which);
        if (playerIsInvalid(player)) {
            break;
        }
        controllers[static_cast<size_t>(player)].buttons[e.cbutton.button] =
            JUST_DOWN;
        break;
    };
    case SDL_CONTROLLERBUTTONUP: {
        const int player = findPlayerSlot(e.cbutton.which);
        if (playerIsInvalid(player)) {
            break;
        }
        controllers[static_cast<size_t>(player)].buttons[e.cbutton.button] =
            JUST_UP;
        break;
    }
    case SDL_CONTROLLERDEVICEADDED: {
        // e.cdevice.which is a device index here, not an instance ID. We
        // don't rely on SDL's own player-index auto-assignment: on the
        // Switch backend, all controllers are announced through a bootstrap
        // path (SDL_GameControllerInit's initial scan) that never calls the
        // function that assigns a player index, so
        // SDL_GameControllerGetPlayerIndex would return -1 forever. We
        // track player slots ourselves instead, keyed by instance ID, which
        // is reliably present on every backend.
        SDL_GameController *controller =
            SDL_GameControllerOpen(e.cdevice.which);
        if (controller == nullptr) {
            break;
        }
        const SDL_JoystickID instanceId =
            SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(controller));
        const int slot = allocatePlayerSlot(instanceId);
        if (slot == -1) {
            SDL_GameControllerClose(controller);
            break;
        }
        SDL_GameControllerSetPlayerIndex(controller, slot); // cosmetic (LED) only
        break;
    }
    case SDL_CONTROLLERDEVICEREMOVED: {
        // e.cdevice.which is an instance ID here, unlike on device added
        SDL_GameController *controller =
            SDL_GameControllerFromInstanceID(e.cdevice.which);
        freePlayerSlot(e.cdevice.which);
        SDL_GameControllerClose(controller);
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
    int count = 0;
    for (SDL_JoystickID id : slotInstanceId) {
        if (id != -1) {
            ++count;
        }
    }
    return count;
}

void InputState::resetFrame() {
    keyboard.resetFrame();
    mouse.resetFrame();
    for (ControllerState &controller : controllers) {
        controller.resetFrame();
    }
}
