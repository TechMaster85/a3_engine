#include "renderer.h"

#include <SDL_render.h>
#include <SDL_blendmode.h>
#include "glm/common.hpp"

#include "Helper.h"
#include "glm/fwd.hpp"
#include "rendering/imagedb.h"
#include "renderqueue.h"

#include <algorithm>
#include <cstdint>
#include <cassert>

constexpr int PIXELS_PER_UNIT = 100;

void Renderer::draw(const std::string &image_name, float x, float y) {
    ImageDrawRequest request;
    request.texture = ImageDB::get(image_name);
    request.x = x;
    request.y = y;
    Helper::SDL_QueryTexture(request.texture, &request.texture_width,
                             &request.texture_height);
    imageQueue.emplace_back(request);
}

void Renderer::drawEx(const std::string &image_name, float x, float y,
                      float rotation_degrees, float scale_x, float scale_y,
                      float pivot_x, float pivot_y, float r, float g, float b,
                      float a, float sorting_order) {
    ImageDrawRequest request;
    request.texture = ImageDB::get(image_name);
    request.x = x;
    request.y = y;
    request.rotation_degrees = rotation_degrees;
    request.scale_x = scale_x;
    request.scale_y = scale_y;
    request.pivot_x = pivot_x;
    request.pivot_y = pivot_y;
    request.sorting_order = static_cast<int>(sorting_order);
    request.r = static_cast<uint8_t>(r);
    request.g = static_cast<uint8_t>(g);
    request.b = static_cast<uint8_t>(b);
    request.a = static_cast<uint8_t>(a);
    Helper::SDL_QueryTexture(request.texture, &request.texture_width,
                             &request.texture_height);
    imageQueue.emplace_back(request);
}

void Renderer::drawUi(const std::string &image_name, float x, float y) {
    ImageDrawRequest request;
    request.texture = ImageDB::get(image_name);
    request.x = x;
    request.y = y;
    Helper::SDL_QueryTexture(request.texture, &request.texture_width,
                             &request.texture_height);
    imageUiQueue.emplace_back(request);
}

void Renderer::drawUiEx(const std::string &image_name, float x, float y,
                        float r, float g, float b, float a,
                        float sorting_order) {
    ImageDrawRequest request;
    request.texture = ImageDB::get(image_name);
    request.x = x;
    request.y = y;
    request.sorting_order = static_cast<int>(sorting_order);
    request.r = static_cast<uint8_t>(r);
    request.g = static_cast<uint8_t>(g);
    request.b = static_cast<uint8_t>(b);
    request.a = static_cast<uint8_t>(a);
    Helper::SDL_QueryTexture(request.texture, &request.texture_width,
                             &request.texture_height);
    imageUiQueue.emplace_back(request);
}

void Renderer::drawText(const std::string &str_content, float x, float y,
                        const std::string &font_name, float font_size, float r,
                        float g, float b, float a) {
    textQueue.emplace_back(str_content, x, y, font_name, font_size, r, g, b, a);
}

void Renderer::drawPixel(float x, float y, float r, float g, float b, float a) {
    pixelQueue.emplace_back(x, y, r, g, b, a);
}

void Renderer::update() {
    SDL_RenderClear(renderer);
    pruneQueues();

    std::stable_sort(imageQueue.begin(), imageQueue.end(),
                     [](const ImageDrawRequest &a, const ImageDrawRequest &b) {
                         return a.sorting_order < b.sorting_order;
                     });
    std::stable_sort(imageUiQueue.begin(), imageUiQueue.end(),
                     [](const ImageDrawRequest &a, const ImageDrawRequest &b) {
                         return a.sorting_order < b.sorting_order;
                     });

    SDL_RenderSetScale(renderer, zoom, zoom);
    for (const ImageDrawRequest &request : imageQueue) {
        assert(request.texture_width > 0.0F && request.texture_height > 0.0F);

        SDL_SetTextureColorMod(request.texture, request.r, request.g,
                               request.b);
        SDL_SetTextureAlphaMod(request.texture, request.a);

        const float screen_x =
            ((request.x - cameraPosition.x) * PIXELS_PER_UNIT) +
            (windowResolution.x / 2.0F / zoom);
        const float screen_y =
            ((request.y - cameraPosition.y) * PIXELS_PER_UNIT) +
            (windowResolution.y / 2.0F / zoom);

        SDL_FRect rect;
        rect.w = request.texture_width;
        rect.h = request.texture_height;
        rect.w *= glm::abs(request.scale_x);
        rect.h *= glm::abs(request.scale_y);
        rect.x = screen_x - (request.pivot_x * rect.w);
        rect.y = screen_y - (request.pivot_y * rect.h);

        const SDL_FPoint center{request.pivot_x * rect.w,
                                request.pivot_y * rect.h};

        SDL_RendererFlip flip = SDL_FLIP_NONE;
        if (request.scale_x < 0 && request.scale_y < 0) {
            flip = static_cast<SDL_RendererFlip>(SDL_FLIP_HORIZONTAL |
                                                 SDL_FLIP_VERTICAL);
        } else if (request.scale_x < 0) {
            flip = SDL_FLIP_HORIZONTAL;
        } else if (request.scale_y < 0) {
            flip = SDL_FLIP_VERTICAL;
        }

        Helper::SDL_RenderCopyEx(0, "", renderer, request.texture, nullptr,
                                 &rect, request.rotation_degrees, &center,
                                 flip);
    }
    SDL_RenderSetScale(renderer, 1, 1);

    for (const ImageDrawRequest &request : imageUiQueue) {
        SDL_SetTextureColorMod(request.texture, request.r, request.g,
                               request.b);
        SDL_SetTextureAlphaMod(request.texture, request.a);
        SDL_FRect rect;
        rect.x = request.x;
        rect.y = request.y;
        rect.w = request.texture_width;
        rect.h = request.texture_height;
        Helper::SDL_RenderCopy(renderer, request.texture, nullptr, &rect);
    }

    for (const TextDrawRequest &request : textQueue) {
        SDL_SetTextureColorMod(request.texture, request.r, request.g,
                               request.b);
        SDL_SetTextureAlphaMod(request.texture, request.a);
        SDL_FRect rect;
        rect.x = request.x;
        rect.y = request.y;
        Helper::SDL_QueryTexture(request.texture, &rect.w, &rect.h);
        Helper::SDL_RenderCopy(renderer, request.texture, nullptr, &rect);
    }

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    for (const PixelDrawRequest &request : pixelQueue) {
        SDL_SetRenderDrawColor(renderer, request.r, request.g, request.b,
                               request.a);
        SDL_RenderDrawPoint(renderer, request.x, request.y);
    }
    SDL_SetRenderDrawColor(renderer, clearColor.r, clearColor.g, clearColor.b,
                           clearColor.a);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

    imageQueue.clear();
    imageUiQueue.clear();
    textQueue.clear();
    pixelQueue.clear();

    Helper::SDL_RenderPresent(renderer);
}

void Renderer::pruneQueues() {
    const float half_w_world =
        (windowResolution.x / 2.0F / zoom) / PIXELS_PER_UNIT;
    const float half_h_world =
        (windowResolution.y / 2.0F / zoom) / PIXELS_PER_UNIT;

    imageQueue.erase(
        std::remove_if(
            imageQueue.begin(), imageQueue.end(),
            [half_w_world, half_h_world,
             cam = cameraPosition](const ImageDrawRequest &request) {
                const float world_w =
                    (request.texture_width * glm::abs(request.scale_x)) /
                    PIXELS_PER_UNIT;
                const float world_h =
                    (request.texture_height * glm::abs(request.scale_y)) /
                    PIXELS_PER_UNIT;

                // Sprite center in world space, accounting for pivot offset
                // from anchor
                const float center_x =
                    request.x + ((0.5F - request.pivot_x) * world_w);
                const float center_y =
                    request.y + ((0.5F - request.pivot_y) * world_h);

                // Half-diagonal is the max distance from center to any corner
                // at any rotation
                const float half_diag =
                    std::sqrt((world_w * world_w) + (world_h * world_h)) / 2.0f;

                return center_x + half_diag < cam.x - half_w_world ||
                       center_x - half_diag > cam.x + half_w_world ||
                       center_y + half_diag < cam.y - half_h_world ||
                       center_y - half_diag > cam.y + half_h_world;
            }),
        imageQueue.end());
}
