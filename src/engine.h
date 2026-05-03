#pragma once

class Engine {
public:
    // Functions to call in main.cpp
    static void startup();
    static void loop();

    static int getFrameNumber() { return frameNumber; }

private:
    // Loop functions
    static void input();
    static void update();
    static void render();

    static inline bool running = true;
    static inline int frameNumber = 0;
};
