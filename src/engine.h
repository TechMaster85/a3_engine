#pragma once

class Engine {
public:
    // Functions to call in main.cpp
    static void startup();
    static void loop();

private:
    // Loop functions
    static void input();
    static void update();
    static void render();

    static inline bool running = true;
};
