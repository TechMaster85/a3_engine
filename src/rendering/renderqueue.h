#pragma once

#include "Helper.h"
#include "textdb.h"

#include <cstdint>
#include <string>

struct SDL_Texture;

struct ImageDrawRequest {
    SDL_Texture *texture = nullptr;
    float texture_width = -1;  // Don't forget to update on adding
    float texture_height = -1; // Don't forget to update on adding
    float x = 0;
    float y = 0;
    float rotation_degrees = 0;
    float scale_x = 1.0F;
    float scale_y = 1.0F;
    float pivot_x = 0.5F;
    float pivot_y = 0.5F;
    int sorting_order = 0;
    uint8_t r = UINT8_MAX;
    uint8_t g = UINT8_MAX;
    uint8_t b = UINT8_MAX;
    uint8_t a = UINT8_MAX;
};

struct TextDrawRequest {
    SDL_Texture *texture;
    float x;
    float y;
    int sorting_order = 0;
    uint8_t r = UINT8_MAX;
    uint8_t g = UINT8_MAX;
    uint8_t b = UINT8_MAX;
    uint8_t a = UINT8_MAX;

    explicit TextDrawRequest(const std::string &str_content, float x, float y,
                             const std::string &font_name, float font_size,
                             float r, float g, float b, float a)
        : texture(TextDB::getStringTexture(str_content, font_name,
                                           static_cast<int>(font_size))),
          x(x), y(y), r(static_cast<uint8_t>(r)), g(static_cast<uint8_t>(g)),
          b(static_cast<uint8_t>(b)), a(static_cast<uint8_t>(a)) {}
};

struct PixelDrawRequest {
    int x;
    int y;
    uint8_t r = UINT8_MAX;
    uint8_t g = UINT8_MAX;
    uint8_t b = UINT8_MAX;
    uint8_t a = UINT8_MAX;

    explicit PixelDrawRequest(float x, float y, float r, float g, float b,
                              float a)
        : x(static_cast<int>(x)), y(static_cast<int>(y)),
          r(static_cast<uint8_t>(r)), g(static_cast<uint8_t>(g)),
          b(static_cast<uint8_t>(b)), a(static_cast<uint8_t>(a)) {}
};
