#include "renderer.h"

#include "config.h"
#include "core/fileutil.h"
#include "core/jsonutil.h"

#include <filesystem>

Renderer::Renderer() {
    imageQueue.reserve(100000);

    if (std::filesystem::exists(FileUtil::RENDERING_CONFIG_PATH)) {
        const rapidjson::Document &renderingConfig =
            JsonUtil::loadJsonFile(FileUtil::RENDERING_CONFIG_PATH);

        JsonUtil::get(renderingConfig, "x_resolution", windowResolution.x);
        JsonUtil::get(renderingConfig, "y_resolution", windowResolution.y);
        JsonUtil::get(renderingConfig, "clear_color_r", clearColor.r);
        JsonUtil::get(renderingConfig, "clear_color_g", clearColor.g);
        JsonUtil::get(renderingConfig, "clear_color_b", clearColor.b);
    }

    SDL_Window *newWindow = SDL_CreateWindow(
        Config::gameTitle.c_str(), 0, 0, static_cast<int>(windowResolution.x),
        static_cast<int>(windowResolution.y), 0);
    window = newWindow;

    renderer = SDL_CreateRenderer(
        newWindow, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);

    SDL_SetRenderDrawColor(renderer, clearColor.r, clearColor.g, clearColor.b,
                           clearColor.a);
    SDL_RenderClear(renderer);
}
