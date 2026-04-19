#pragma once

#include <string>
#include <unordered_map>

struct SDL_Renderer;
struct SDL_Texture;

class ImageDB {
public:
    static SDL_Texture *get(const std::string &name);

private:
    static inline std::unordered_map<std::string, SDL_Texture *> nameToTexture;
    static inline SDL_Texture *defaultParticle;
};
