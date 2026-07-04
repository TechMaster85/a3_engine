#include "input.h"

#include <SDL_events.h>
#include <SDL_gamecontroller.h>
#include <SDL_scancode.h>

#include <cstdint>
#include <string>
#include <unordered_map>

namespace {
const std::unordered_map<std::string, SDL_Scancode> keycodeToScancode = {
    // Directional (arrow) Keys
    {"up", SDL_SCANCODE_UP},
    {"down", SDL_SCANCODE_DOWN},
    {"right", SDL_SCANCODE_RIGHT},
    {"left", SDL_SCANCODE_LEFT},

    // Misc Keys
    {"escape", SDL_SCANCODE_ESCAPE},

    // Modifier Keys
    {"lshift", SDL_SCANCODE_LSHIFT},
    {"rshift", SDL_SCANCODE_RSHIFT},
    {"lctrl", SDL_SCANCODE_LCTRL},
    {"rctrl", SDL_SCANCODE_RCTRL},
    {"lalt", SDL_SCANCODE_LALT},
    {"ralt", SDL_SCANCODE_RALT},

    // Editing Keys
    {"tab", SDL_SCANCODE_TAB},
    {"return", SDL_SCANCODE_RETURN},
    {"enter", SDL_SCANCODE_RETURN},
    {"backspace", SDL_SCANCODE_BACKSPACE},
    {"delete", SDL_SCANCODE_DELETE},
    {"insert", SDL_SCANCODE_INSERT},

    // Character Keys
    {"space", SDL_SCANCODE_SPACE},
    {"a", SDL_SCANCODE_A},
    {"b", SDL_SCANCODE_B},
    {"c", SDL_SCANCODE_C},
    {"d", SDL_SCANCODE_D},
    {"e", SDL_SCANCODE_E},
    {"f", SDL_SCANCODE_F},
    {"g", SDL_SCANCODE_G},
    {"h", SDL_SCANCODE_H},
    {"i", SDL_SCANCODE_I},
    {"j", SDL_SCANCODE_J},
    {"k", SDL_SCANCODE_K},
    {"l", SDL_SCANCODE_L},
    {"m", SDL_SCANCODE_M},
    {"n", SDL_SCANCODE_N},
    {"o", SDL_SCANCODE_O},
    {"p", SDL_SCANCODE_P},
    {"q", SDL_SCANCODE_Q},
    {"r", SDL_SCANCODE_R},
    {"s", SDL_SCANCODE_S},
    {"t", SDL_SCANCODE_T},
    {"u", SDL_SCANCODE_U},
    {"v", SDL_SCANCODE_V},
    {"w", SDL_SCANCODE_W},
    {"x", SDL_SCANCODE_X},
    {"y", SDL_SCANCODE_Y},
    {"z", SDL_SCANCODE_Z},
    {"0", SDL_SCANCODE_0},
    {"1", SDL_SCANCODE_1},
    {"2", SDL_SCANCODE_2},
    {"3", SDL_SCANCODE_3},
    {"4", SDL_SCANCODE_4},
    {"5", SDL_SCANCODE_5},
    {"6", SDL_SCANCODE_6},
    {"7", SDL_SCANCODE_7},
    {"8", SDL_SCANCODE_8},
    {"9", SDL_SCANCODE_9},
    {"/", SDL_SCANCODE_SLASH},
    {";", SDL_SCANCODE_SEMICOLON},
    {"=", SDL_SCANCODE_EQUALS},
    {"-", SDL_SCANCODE_MINUS},
    {".", SDL_SCANCODE_PERIOD},
    {",", SDL_SCANCODE_COMMA},
    {"[", SDL_SCANCODE_LEFTBRACKET},
    {"]", SDL_SCANCODE_RIGHTBRACKET},
    {"\\", SDL_SCANCODE_BACKSLASH},
    {"'", SDL_SCANCODE_APOSTROPHE}};

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
