#include "imagedb.h"

#include "filepath.h"
#include "renderer.h"

#include <SDL_render.h>
#include <SDL_image.h>
#include <SDL_stdinc.h>
#include <SDL_surface.h>

#include <cassert>
#include <filesystem>
#include <iostream>

SDL_Texture *ImageDB::get(const std::string &name) {
    if (name.empty()) {
        if (defaultParticle != nullptr) {
            return defaultParticle;
        }

        SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormat(
            0, 8, 8, 32, SDL_PIXELFORMAT_RGBA8888);
        Uint32 whiteColor = SDL_MapRGBA(surface->format, 255, 255, 255, 255);
        SDL_FillRect(surface, nullptr, whiteColor);

        defaultParticle =
            SDL_CreateTextureFromSurface(Renderer::getRenderer(), surface);
        SDL_FreeSurface(surface);
        return defaultParticle;
    }

    const auto it = nameToTexture.find(name);
    if (it != nameToTexture.end()) {
        return it->second;
    }

    const std::filesystem::path imagePath = getImagePath(name);
    if (!std::filesystem::exists(imagePath)) {
        std::cout << "error: missing image " << name;
        exit(0);
    }

    SDL_Texture *texture =
        IMG_LoadTexture(Renderer::getRenderer(), imagePath.c_str());
    nameToTexture[name] = texture;
    return texture;
}
