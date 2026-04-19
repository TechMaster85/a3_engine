#pragma once

#include "renderqueue.h"

#include "glm/ext/vector_float2.hpp"
#include <vector>

struct SDL_Window;
struct SDL_Renderer;
struct SDL_Texture;

class Renderer {
public:
    explicit Renderer();
    static SDL_Renderer *getRenderer() { return renderer; }
    static glm::vec2 getResolution() { return windowResolution; }

    // Put request directly in queue for particles
    static void drawRawImage(const ImageDrawRequest &r) {
        imageQueue.emplace_back(r);
    }

    // Lua drawing bindings
    static void draw(const std::string &image_name, float x, float y);
    static void drawEx(const std::string &image_name, float x, float y,
                       float rotation_degrees, float scale_x, float scale_y,
                       float pivot_x, float pivot_y, float r, float g, float b,
                       float a, float sorting_order);
    static void drawUi(const std::string &image_name, float x, float y);
    static void drawUiEx(const std::string &image_name, float x, float y,
                         float r, float g, float b, float a,
                         float sorting_order);
    static void drawText(const std::string &str_content, float x, float y,
                         const std::string &font_name, float font_size, float r,
                         float g, float b, float a);
    static void drawPixel(float x, float y, float r, float g, float b, float a);

    // Lua camera bindings
    static void setPosition(float x, float y) { cameraPosition = {x, y}; }
    static float getPositionX() { return cameraPosition.x; }
    static float getPositionY() { return cameraPosition.y; }
    static void setZoom(float z) { zoom = z; }
    static float getZoom() { return zoom; }

    // In game loop
    static void pruneQueues();
    static void update();

private:
    static constexpr glm::vec2 DEFAULT_RESOLUTION = {640.0F, 360.0F};

    // Variables required for window
    static inline SDL_Window *window = nullptr;
    static inline SDL_Renderer *renderer = nullptr;
    static inline SDL_Color clearColor = {0, 0, 0, UINT8_MAX};
    static inline glm::vec2 windowResolution = DEFAULT_RESOLUTION;

    // Camera info
    static inline glm::vec2 cameraPosition;
    static inline float zoom = 1.0F;

    // Drawing queue
    static inline std::vector<ImageDrawRequest> imageQueue;
    static inline std::vector<ImageDrawRequest> imageUiQueue;
    static inline std::vector<TextDrawRequest> textQueue;
    static inline std::vector<PixelDrawRequest> pixelQueue;
};
