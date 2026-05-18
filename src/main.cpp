#include "engine.h"

int main(int argc, char *argv[]) {
    (void)argc;
    (void)argv;

    Engine::startup();
    Engine::loop();

    return 0;
}
