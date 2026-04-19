#include "engine.h"

#include <cstdlib>

int main(int argc, char *argv[]) {
    (void)argc;
    (void)argv;

    Engine engine;
    engine.startup();
    engine.loop();

    return 0;
}
