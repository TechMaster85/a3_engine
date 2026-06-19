#include "input/input.h"

#include <SDL_gamecontroller.h>
#include <SDL_joystick.h>

void Input::initializeController() {
    int slot = 0;
    for (int i = 0; i < SDL_NumJoysticks() && slot < 4; ++i) {
        if (SDL_IsGameController(i) == SDL_TRUE) {
            SDL_GameController* handle = SDL_GameControllerOpen(i);
            if (!handle) continue;
            SDL_JoystickID id = SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(handle));
            controllers[static_cast<std::size_t>(slot)].handle = handle;
            controllers[static_cast<std::size_t>(slot)].connected = true;
            instanceToSlot[id] = slot;
            ++slot;
        }
    }
}
