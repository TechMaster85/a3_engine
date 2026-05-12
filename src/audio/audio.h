#pragma once

#include <string>
#include <unordered_map>

struct lua_State;
struct Mix_Chunk;

class Audio {
public:
    explicit Audio();
    static void play(int channel, const char *name, bool doesLoop);
    static void halt(int channel);
    static void setVolume(int channel, float volume);

    static void registerLuaBindings(lua_State *L);

private:
    static inline std::unordered_map<std::string, Mix_Chunk *> chunkCache;
};
