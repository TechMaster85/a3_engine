#include "audio.h"

#include <cassert>
#include <filesystem>
#include <iostream>

#include "AudioHelper.h"
#include "core/filepath.h"

#include <SDL_mixer.h>

// The SDL version that devkitpro on Switch uses does not have Mix_PlayChannel
// or Mix_LoadWAV, so this replaces the functions automatically
#ifdef __SWITCH__
#include <SDL2/SDL_mixer.h>
#undef Mix_PlayChannel
#undef Mix_LoadWAV

extern "C" {
int Mix_PlayChannel(int channel, Mix_Chunk *chunk, int loops) {
    return Mix_PlayChannelTimed(channel, chunk, loops, -1);
}
Mix_Chunk *Mix_LoadWAV(const char *file) {
    return Mix_LoadWAV_RW(SDL_RWFromFile(file, "rb"), 1);
}
}
#endif

Audio::Audio() {
    constexpr int SAMPLE_RATE = 48000;
    constexpr int BUFFER_SIZE = 1024;
    constexpr int NUM_CHANNELS = 50;
    AudioHelper::Mix_OpenAudio(SAMPLE_RATE, AUDIO_S16SYS, 2, BUFFER_SIZE);
    AudioHelper::Mix_AllocateChannels(NUM_CHANNELS);
};

void Audio::play(int channel, const std::string &name, bool doesLoop) {
    const int loopSetting = doesLoop ? -1 : 0;
    if (Mix_Playing(channel) != 0) {
        halt(channel);
    }

    if (chunkCache.count(name) > 0) {
        AudioHelper::Mix_PlayChannel(channel, chunkCache[name], loopSetting);
        return;
    }

    const std::filesystem::path filePathOgg = getAudioOggPath(name);
    const std::filesystem::path filePathWav = getAudioWavPath(name);
    const bool oggExists = std::filesystem::exists(filePathOgg);
    const bool wavExists = std::filesystem::exists(filePathWav);

    if (!oggExists && !wavExists) {
        std::cout << "error: failed to play audio clip " << name;
        exit(1);
    }

    const std::filesystem::path &filePath = oggExists ? filePathOgg : filePathWav;

    chunkCache[name] = AudioHelper::Mix_LoadWAV(filePath.string().c_str());
    AudioHelper::Mix_PlayChannel(channel, chunkCache[name], loopSetting);
}

void Audio::halt(int channel) { AudioHelper::Mix_HaltChannel(channel); }

void Audio::setVolume(int channel, float volume) {
    AudioHelper::Mix_Volume(channel, static_cast<int>(volume));
}
