#pragma once

struct lua_State;

class Engine {
public:
    static inline lua_State *L = nullptr;

    static void startup();
    static void loop();

    static int getFrameNumber() { return frameNumber; }

private:
    static void input();
    static void update();
    static void render();

    static inline bool running = true;
    static inline int frameNumber = 0;
};
