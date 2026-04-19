#include "input/input.h"

#include <SDL_gamecontroller.h>

void Input::initializeController() {
    for (int i = 0; i < SDL_NumJoysticks(); ++i) {
        if (SDL_IsGameController(i) == SDL_TRUE) {
            controller = SDL_GameControllerOpen(i);
        }
    }
}
