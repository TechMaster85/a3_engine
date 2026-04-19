#pragma once

#include "Helper.h"

#include "rapidjson/document.h"

#include <cstdint>
#include <optional>
#include <queue>

struct Actor;
struct SDL_Texture;

struct ParticleSystemProperties {
    // Static properties
    float x = 0.0F;
    float y = 0.0F;
    int frames_between_bursts = 1; // If below 1, engine treats as 1
    int burst_quantity = 1;        // If below 1, engine treats as 1
    float start_scale_min = 1.0F;
    float start_scale_max = 1.0F;
    float rotation_min = 0.0F;
    float rotation_max = 0.0F;
    uint8_t start_color_r = 255;
    uint8_t start_color_g = 255;
    uint8_t start_color_b = 255;
    uint8_t start_color_a = 255;
    float emit_radius_min = 0.0F;
    float emit_radius_max = 0.5F;
    float emit_angle_min = 0.0F;
    float emit_angle_max = 360.0F;
    std::string image;
    int sorting_order = 9999;

    // Dynamic properties
    int duration_frames = 300; // If below 1, engine treats as 1
    float start_speed_min = 0.0F;
    float start_speed_max = 0.0F;
    float rotation_speed_min = 0.0F;
    float rotation_speed_max = 0.0F;
    float gravity_scale_x = 0.0F;
    float gravity_scale_y = 0.0F;
    float drag_factor = 1.0F;
    float angular_drag_factor = 1.0F;
    std::optional<float> end_scale;
    std::optional<uint8_t> end_color_r;
    std::optional<uint8_t> end_color_g;
    std::optional<uint8_t> end_color_b;
    std::optional<uint8_t> end_color_a;

    static ParticleSystemProperties fromJson(const rapidjson::Value &json);
};

struct ParticleSystem {
    ParticleSystem(ParticleSystemProperties p, std::string name, Actor *actor);

    ParticleSystemProperties properties;
    std::string name;
    Actor *actor;
    bool enabled = true;
    bool emissionAllowed = true;

    int frameNumber = 0;
    SDL_Texture *texture = nullptr;
    float textureWidth = 0.0F;
    float textureHeight = 0.0F;

    std::queue<size_t> freeSlots;
    std::vector<float> posX;
    std::vector<float> posY;
    std::vector<float> velX;
    std::vector<float> velY;
    std::vector<float> rotations;
    std::vector<float> rotationSpeeds;
    std::vector<float> startScales;
    std::vector<int> ages;

    void onStart();
    void onUpdate();
    void burst();
    void stop() { emissionAllowed = false; };
    void play() { emissionAllowed = true; };

    [[nodiscard]] float getX() const { return properties.x; }
    void setX(float x) { properties.x = x; }
    [[nodiscard]] float getY() const { return properties.y; }
    void setY(float y) { properties.y = y; }
    std::string getType() const { return "ParticleSystem"; }

    [[nodiscard]] int getStartColorR() const { return properties.start_color_r; }
    void setStartColorR(int r) { properties.start_color_r = static_cast<uint8_t>(r); }
    [[nodiscard]] int getStartColorG() const { return properties.start_color_g; }
    void setStartColorG(int g) { properties.start_color_g = static_cast<uint8_t>(g); }
    [[nodiscard]] int getStartColorB() const { return properties.start_color_b; }
    void setStartColorB(int b) { properties.start_color_b = static_cast<uint8_t>(b); }

    RandomEngine emit_angle_distribution =
        RandomEngine(properties.emit_angle_min, properties.emit_angle_max, 298);
    RandomEngine emit_radius_distribution = RandomEngine(
        properties.emit_radius_min, properties.emit_radius_max, 404);
    RandomEngine rotation_distribution =
        RandomEngine(properties.rotation_min, properties.rotation_max, 440);
    RandomEngine scale_distribution = RandomEngine(
        properties.start_scale_min, properties.start_scale_max, 494);
    RandomEngine speed_distribution = RandomEngine(
        properties.start_speed_min, properties.start_speed_max, 498);
    RandomEngine rotation_speed_distribution = RandomEngine(
        properties.rotation_speed_min, properties.rotation_speed_max, 305);
};
