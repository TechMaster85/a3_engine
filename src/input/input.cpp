#include "input.h"

#include "actor.h"
#include "config.h"
#include "rendering/renderer.h"

#include <SDL_events.h>

#include <SDL_gamecontroller.h>
#include <SDL_scancode.h>
#include <cmath>
#include <cstdint>
#include <string>
#include <unordered_map>

namespace {
inline void setKeyToHold(Input::KeyState &k) {
    // JUST_DOWN and JUST_UP are enums 1 and 3, respectively
    const uint8_t justOn = (k & 1);
    k = static_cast<Input::KeyState>((k + justOn) & 0b11);
}

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

const std::unordered_map<SDL_GameControllerButton, SDL_Scancode>
    controllerToKeyboard{
        {SDL_CONTROLLER_BUTTON_DPAD_UP, SDL_SCANCODE_UP},
        {SDL_CONTROLLER_BUTTON_DPAD_DOWN, SDL_SCANCODE_DOWN},
        {SDL_CONTROLLER_BUTTON_DPAD_LEFT, SDL_SCANCODE_LEFT},
        {SDL_CONTROLLER_BUTTON_DPAD_RIGHT, SDL_SCANCODE_RIGHT}};
} // namespace

void Input::resetFrame() {
    for (KeyState &k : keys) {
        setKeyToHold(k);
    }
    for (KeyState &k : mouseKeys) {
        setKeyToHold(k);
    }
    scrollDelta = rightStickY * 0.15F;
}

void Input::handleEvent(SDL_Event &e) {
    switch (e.type) {
    case SDL_KEYDOWN:
        keys[e.key.keysym.scancode] = JUST_DOWN;
        break;
    case SDL_KEYUP:
        keys[e.key.keysym.scancode] = JUST_UP;
        break;
    case SDL_MOUSEBUTTONDOWN:
        mouseKeys[e.button.button] = JUST_DOWN;
        break;
    case SDL_MOUSEBUTTONUP:
        mouseKeys[e.button.button] = JUST_UP;
        break;
    case SDL_MOUSEMOTION:
        mousePosition = {e.motion.x, e.motion.y};
        break;
    case SDL_MOUSEWHEEL:
        scrollDelta += e.wheel.preciseY;
        break;
    case SDL_CONTROLLERAXISMOTION: {
        if (e.caxis.axis == SDL_CONTROLLER_AXIS_RIGHTY) {
            const float normalized = e.caxis.value / 32767.0F;
            rightStickY = (std::abs(normalized) > 0.25F) ? -normalized : 0.0F;
        }
        break;
    }
    case SDL_CONTROLLERBUTTONDOWN: {
        auto it = controllerToKeyboard.find(
            static_cast<SDL_GameControllerButton>(e.cbutton.button));
        if (it != controllerToKeyboard.end())
            keys[it->second] = JUST_DOWN;
        break;
    }
    case SDL_CONTROLLERBUTTONUP: {
        auto it = controllerToKeyboard.find(
            static_cast<SDL_GameControllerButton>(e.cbutton.button));
        if (it != controllerToKeyboard.end())
            keys[it->second] = JUST_UP;
        break;
    }
    case SDL_FINGERDOWN:
        mousePosition = {e.tfinger.x * Renderer::getResolution().x,
                         e.tfinger.y * Renderer::getResolution().y};
        mouseKeys[1] = JUST_DOWN;
        break;
    case SDL_FINGERUP:
        mouseKeys[1] = JUST_UP;
        break;
    case SDL_FINGERMOTION:
        mousePosition = {e.tfinger.x * Renderer::getResolution().x,
                         e.tfinger.y * Renderer::getResolution().y};
        break;

    default:
        break;
    }
}

// View keyboard
bool Input::getKey(const std::string &keycode) {
    auto it = keycodeToScancode.find(keycode);
    if (it == keycodeToScancode.end()) {
        return false;
    }
    return keys[it->second] == DOWN || keys[it->second] == JUST_DOWN;
}
bool Input::getKeyDown(const std::string &keycode) {
    auto it = keycodeToScancode.find(keycode);
    if (it == keycodeToScancode.end()) {
        return false;
    }
    return keys[it->second] == JUST_DOWN;
}
bool Input::getKeyUp(const std::string &keycode) {
    auto it = keycodeToScancode.find(keycode);
    if (it == keycodeToScancode.end()) {
        return false;
    }
    return keys[it->second] == JUST_UP;
}
