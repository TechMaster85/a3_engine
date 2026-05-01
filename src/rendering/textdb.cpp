#include "textdb.h"

#include "filepath.h"
#include "renderer.h"

#include <SDL_ttf.h>

#include <cassert>
#include <filesystem>
#include <iostream>

TextDB::TextDB() { TTF_Init(); }

SDL_Texture *TextDB::getStringTexture(const std::string &contents, // NOLINT
                                      const std::string &fontName,
                                      int fontSize) {
    const std::string fontKey = fontName + "|" + std::to_string(fontSize);
    TTF_Font *ttfFont = nullptr;
    const auto fontIt = fontCache.find(fontKey);
    if (fontIt != fontCache.end()) {
        ttfFont = fontIt->second;
    } else {
        const std::filesystem::path fontPath = getFontPath(fontName);
        if (!std::filesystem::exists(fontPath)) {
            std::cout << "error: font " << fontName << " missing";
            exit(1);
        }

        ttfFont = TTF_OpenFont(fontPath.string().c_str(), fontSize);
        fontCache[fontKey] = ttfFont;
    }

    assert(ttfFont != nullptr);
    const std::string textureKey =
        std::to_string(reinterpret_cast<uintptr_t>(ttfFont)) + "|" + contents;
    const auto textureIt = textureCache.find(textureKey);
    if (textureIt != textureCache.end()) {
        return textureIt->second;
    }

    const SDL_Color white = {255, 255, 255, 255};
    SDL_Surface *surface =
        TTF_RenderText_Solid(ttfFont, contents.c_str(), white);
    SDL_Texture *texture =
        SDL_CreateTextureFromSurface(Renderer::getRenderer(), surface);
    SDL_FreeSurface(surface);
    textureCache[textureKey] = texture;
    return texture;
}
