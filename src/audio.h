#pragma once

#include <string>
#include <unordered_map>

struct Mix_Chunk;

class Audio {
public:
    explicit Audio();
    static void play(int channel, const std::string &name, bool doesLoop);
    static void halt(int channel);
    static void setVolume(int channel, float volume);

private:
    static inline std::unordered_map<std::string, Mix_Chunk *> chunkCache;
};
