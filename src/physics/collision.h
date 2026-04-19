#pragma once

#include "box2d/b2_math.h"
#include <vector>

struct Actor;

struct Collision {
    Actor *other = nullptr;
    b2Vec2 point;
    b2Vec2 relative_velocity;
    b2Vec2 normal;
};

struct PendingCollisionEvent {
    Actor *receiver = nullptr;
    bool isEnter = true;
    Collision collision;
};

struct ContactQueue {
    inline static std::vector<PendingCollisionEvent> collisions;
    inline static std::vector<PendingCollisionEvent> triggers;
};
