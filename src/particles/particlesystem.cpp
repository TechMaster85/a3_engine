#include "particlesystem.h"

#include "core/jsonutil.h"
#include "rendering/imagedb.h"
#include "rendering/renderer.h"
#include "rendering/renderqueue.h"

#include <glm/common.hpp>
#include <glm/trigonometric.hpp>

#include <SDL_render.h>

#include <algorithm>
#include <cmath>

namespace {
inline float mix(float start, float end, int curAge, int endAge) {
    if (start == end) {
        return start;
    }

    const float progress =
        static_cast<float>(curAge) / static_cast<float>(endAge);

    return glm::mix(start, end, progress);
}
} // namespace

ParticleSystemProperties
ParticleSystemProperties::fromJson(const rapidjson::Value &json) {
    ParticleSystemProperties psp;
    JsonUtil::get(json, "x", psp.x);
    JsonUtil::get(json, "y", psp.y);
    JsonUtil::get(json, "frames_between_bursts", psp.frames_between_bursts);
    JsonUtil::get(json, "burst_quantity", psp.burst_quantity);
    JsonUtil::get(json, "start_scale_min", psp.start_scale_min);
    JsonUtil::get(json, "start_scale_max", psp.start_scale_max);
    JsonUtil::get(json, "rotation_min", psp.rotation_min);
    JsonUtil::get(json, "rotation_max", psp.rotation_max);
    JsonUtil::get(json, "start_color_r", psp.start_color_r);
    JsonUtil::get(json, "start_color_g", psp.start_color_g);
    JsonUtil::get(json, "start_color_b", psp.start_color_b);
    JsonUtil::get(json, "start_color_a", psp.start_color_a);
    JsonUtil::get(json, "emit_radius_min", psp.emit_radius_min);
    JsonUtil::get(json, "emit_radius_max", psp.emit_radius_max);
    JsonUtil::get(json, "emit_angle_min", psp.emit_angle_min);
    JsonUtil::get(json, "emit_angle_max", psp.emit_angle_max);
    JsonUtil::get(json, "image", psp.image);
    JsonUtil::get(json, "sorting_order", psp.sorting_order);
    JsonUtil::get(json, "duration_frames", psp.duration_frames);
    JsonUtil::get(json, "start_speed_min", psp.start_speed_min);
    JsonUtil::get(json, "start_speed_max", psp.start_speed_max);
    JsonUtil::get(json, "rotation_speed_min", psp.rotation_speed_min);
    JsonUtil::get(json, "rotation_speed_max", psp.rotation_speed_max);
    JsonUtil::get(json, "gravity_scale_x", psp.gravity_scale_x);
    JsonUtil::get(json, "gravity_scale_y", psp.gravity_scale_y);
    JsonUtil::get(json, "drag_factor", psp.drag_factor);
    JsonUtil::get(json, "angular_drag_factor", psp.angular_drag_factor);
    JsonUtil::get(json, "end_scale", psp.end_scale);
    JsonUtil::get(json, "end_color_r", psp.end_color_r);
    JsonUtil::get(json, "end_color_g", psp.end_color_g);
    JsonUtil::get(json, "end_color_b", psp.end_color_b);
    JsonUtil::get(json, "end_color_a", psp.end_color_a);
    return psp;
}

ParticleSystem::ParticleSystem(ParticleSystemProperties p, std::string name,
                               Actor *actor)
    : properties(std::move(p)), name(std::move(name)), actor(actor),
      texture(ImageDB::get(properties.image)) {
    constexpr size_t RESERVE_SIZE = 1000000;
    posX.reserve(RESERVE_SIZE);
    posY.reserve(RESERVE_SIZE);
    velX.reserve(RESERVE_SIZE);
    velY.reserve(RESERVE_SIZE);
    rotations.reserve(RESERVE_SIZE);
    rotationSpeeds.reserve(RESERVE_SIZE);
    startScales.reserve(RESERVE_SIZE);
    ages.reserve(RESERVE_SIZE);
    SDL_QueryTexture(texture, nullptr, nullptr, &textureWidth, &textureHeight);
}

void ParticleSystem::onUpdate() {
    // Create new particles
    if (!enabled) {
        return;
    }
    if (frameNumber % std::max(properties.frames_between_bursts, 1) == 0 &&
        emissionAllowed) {
        burst();
    }

    for (size_t i = 0; i < ages.size(); ++i) {
        if (ages[i] < 0) {
            continue;
        }

        const int maxDuration = std::max(properties.duration_frames, 1);
        if (ages[i] >= maxDuration) {
            freeSlots.push(i);
            ages[i] = -1;
            continue;
        }

        velX[i] += properties.gravity_scale_x;
        velY[i] += properties.gravity_scale_y;
        velX[i] *= properties.drag_factor;
        velY[i] *= properties.drag_factor;
        rotationSpeeds[i] *= properties.angular_drag_factor;
        posX[i] += velX[i];
        posY[i] += velY[i];
        rotations[i] += rotationSpeeds[i];

        const float scale =
            mix(startScales[i], properties.end_scale.value_or(startScales[i]),
                ages[i], properties.duration_frames);

        ImageDrawRequest request;
        request.texture = texture;
        request.texture_width = static_cast<float>(textureWidth);
        request.texture_height = static_cast<float>(textureHeight);
        request.x = posX[i];
        request.y = posY[i];
        request.rotation_degrees = rotations[i];
        request.scale_x = scale;
        request.scale_y = scale;
        request.sorting_order = properties.sorting_order;
        request.r = static_cast<uint8_t>(
            mix(properties.start_color_r,
                properties.end_color_r.value_or(properties.start_color_r),
                ages[i], properties.duration_frames));
        request.g = static_cast<uint8_t>(
            mix(properties.start_color_g,
                properties.end_color_g.value_or(properties.start_color_g),
                ages[i], properties.duration_frames));
        request.b = static_cast<uint8_t>(
            mix(properties.start_color_b,
                properties.end_color_b.value_or(properties.start_color_b),
                ages[i], properties.duration_frames));
        request.a = static_cast<uint8_t>(
            mix(properties.start_color_a,
                properties.end_color_a.value_or(properties.start_color_a),
                ages[i], properties.duration_frames));
        Renderer::drawRawImage(request);

        ++ages[i];
    }
    ++frameNumber;
}

void ParticleSystem::burst() {
    const int burstTimes = std::max(properties.burst_quantity, 1);
    for (int i = 0; i < burstTimes; ++i) {
        size_t location = ages.size();
        if (!freeSlots.empty()) {
            location = freeSlots.front();
            freeSlots.pop();
        } else {
            posX.push_back(0);
            posY.push_back(0);
            velX.push_back(0);
            velY.push_back(0);
            rotations.push_back(0);
            rotationSpeeds.push_back(0);
            startScales.push_back(0);
            ages.push_back(0);
        }

        const float radians = glm::radians(emit_angle_distribution.Sample());
        const float radius = emit_radius_distribution.Sample();

        const float cosAngle = std::cos(radians);
        const float sinAngle = std::sin(radians);
        const float speed = speed_distribution.Sample();

        posX[location] = properties.x + (cosAngle * radius);
        posY[location] = properties.y + (sinAngle * radius);

        velX[location] = speed * cosAngle;
        velY[location] = speed * sinAngle;

        const float rotation = rotation_distribution.Sample();
        rotations[location] = rotation;

        const float rotationSpeed = rotation_speed_distribution.Sample();
        rotationSpeeds[location] = rotationSpeed;

        const float startScale = scale_distribution.Sample();
        startScales[location] = startScale;
        ages[location] = 0;
    }
}
