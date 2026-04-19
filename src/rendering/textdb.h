#pragma once

#include <string>
#include <unordered_map>

#include <SDL_ttf.h>

struct SDL_Renderer;
struct SDL_Texture;

class TextDB {
public:
    explicit TextDB();

    static SDL_Texture *getStringTexture(const std::string &contents,
                                         const std::string &fontName,
                                         int fontSize);

private:
    static inline std::unordered_map<std::string, TTF_Font *> fontCache;
    static inline std::unordered_map<std::string, SDL_Texture *> textureCache;
};
