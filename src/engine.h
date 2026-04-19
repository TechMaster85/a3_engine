#pragma once

class Engine {
public:
    // Functions to call in main.cpp
    void startup();
    void loop();

private:
    // Loop functions
    void input();
    void update();
    void render();

    static inline bool running = true;
};
