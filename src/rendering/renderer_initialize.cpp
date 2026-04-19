#include "renderer.h"

#include "Helper.h"
#include "config.h"
#include "documentmanager.h"

Renderer::Renderer() {
    imageQueue.reserve(100000);

    const rapidjson::Document &renderingConfig =
        DocumentManager::getRenderingConfig();

    for (const auto &member : renderingConfig.GetObject()) {
        const std::string_view key = member.name.GetString();
        if (key == "x_resolution") {
            windowResolution.x = member.value.GetFloat();
        } else if (key == "y_resolution") {
            windowResolution.y = member.value.GetFloat();
        } else if (key == "clear_color_r") {
            clearColor.r = static_cast<Uint8>(member.value.GetInt());
        } else if (key == "clear_color_g") {
            clearColor.g = static_cast<Uint8>(member.value.GetInt());
        } else if (key == "clear_color_b") {
            clearColor.b = static_cast<Uint8>(member.value.GetInt());
        }
    }

    SDL_Window *newWindow = Helper::SDL_CreateWindow(
        Config::gameTitle.c_str(), 0, 0, static_cast<int>(windowResolution.x),
        static_cast<int>(windowResolution.y), 0);
    window = newWindow;

    renderer = Helper::SDL_CreateRenderer(
        newWindow, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);

    SDL_SetRenderDrawColor(renderer, clearColor.r, clearColor.g, clearColor.b,
                           clearColor.a);
    SDL_RenderClear(renderer);
}
