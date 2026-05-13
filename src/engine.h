#pragma once

#include <string>

struct lua_State;

class Engine {
public:
    static inline lua_State *L = nullptr;

    // Very important
    static void startup();
    static void loop();

    // Lua Debug.Log
    static void log();

    // Lua Application namespace
    static void sleep(int x);
    static int getFrameNumber() { return frameNumber; }
    static void openUrl(const std::string &url);

private:
    static void input();
    static void update();
    static void render();

    static inline bool running = true;
    static inline int frameNumber = 0;
};
