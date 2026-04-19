#pragma once

#include "box2d/b2_body.h"
#include "box2d/b2_common.h"
#include "box2d/b2_contact.h"
#include "box2d/b2_fixture.h"
#include "box2d/b2_math.h"
#include "box2d/b2_world.h"
#include "box2d/b2_world_callbacks.h"
#include "glm/glm.hpp"

#include "physics/collision.h"
#include "rapidjson/document.h"

#include <string>

struct Actor;

struct RigidbodyProperties {
    float x = 0.0F;
    float y = 0.0F;
    std::string body_type = "dynamic";
    bool precise = true;
    float gravity_scale = 1.0F;
    float density = 1.0F;
    float angular_friction = 0.3F;
    float rotation = 0.0F;
    bool has_collider = true;
    bool has_trigger = true;
    std::string collider_type = "box";
    float width = 1.0F;
    float height = 1.0F;
    float radius = 0.5F;
    float friction = 0.3F;
    float bounciness = 0.3F;
    std::string trigger_type = "box";
    float trigger_width = 1.0F;
    float trigger_height = 1.0F;
    float trigger_radius = 0.5F;

    static RigidbodyProperties fromJson(const rapidjson::Value &json);
};

class Rigidbody {
    static constexpr float GRAVITY = 9.8F;

    static void registerContactListener();

    b2Body *body = nullptr;

    std::string key;
    Actor *actor;

public:
    bool enabled = true;
    RigidbodyProperties info;

    static constexpr float degToRad(float deg) {
        return deg * (b2_pi / 180.0F);
    }
    static constexpr float radToDeg(float rad) {
        return rad * (180.0F / b2_pi);
    }

    Rigidbody(RigidbodyProperties info, std::string key, Actor *actor);

    static b2World &getWorld();

    // Properties
    [[nodiscard]] const std::string &getKey() const { return key; }
    [[nodiscard]] Actor *getActor() const { return actor; }
    std::string getType() const { return "Rigidbody"; }

    // Simplified inline getters/setters for Lua property bindings
    float getX() const { return getPosition().x; }
    void setX(float val) { setPosition({val, getY()}); }
    float getY() const { return getPosition().y; }
    void setY(float val) { setPosition({getX(), val}); }
    float getRotationDeg() const { return getRotation(); }
    void setRotationDeg(float deg) { setRotation(deg); }
    std::string getBodyType() const { return info.body_type; }
    void setBodyType(const std::string &val) { info.body_type = val; }
    bool getPrecise() const { return info.precise; }
    void setPrecise(bool val) { info.precise = val; }
    float getGravityScaleVal() const { return getGravityScale(); }
    void setGravityScaleVal(float val) { setGravityScale(val); }
    float getDensity() const { return info.density; }
    void setDensity(float val) { info.density = val; }
    float getAngularFriction() const { return info.angular_friction; }
    void setAngularFriction(float val) { info.angular_friction = val; }
    bool getHasCollider() const { return info.has_collider; }
    void setHasCollider(bool val) { info.has_collider = val; }
    bool getHasTrigger() const { return info.has_trigger; }
    void setHasTrigger(bool val) { info.has_trigger = val; }
    std::string getColliderType() const { return info.collider_type; }
    void setColliderType(const std::string &val) { info.collider_type = val; }
    float getWidth() const { return info.width; }
    void setWidth(float val) { info.width = val; }
    float getHeight() const { return info.height; }
    void setHeight(float val) { info.height = val; }
    float getRadius() const { return info.radius; }
    void setRadius(float val) { info.radius = val; }
    float getFriction() const { return info.friction; }
    void setFriction(float val) { info.friction = val; }
    float getBounciness() const { return info.bounciness; }
    void setBounciness(float val) { info.bounciness = val; }
    std::string getTriggerType() const { return info.trigger_type; }
    void setTriggerType(const std::string &val) { info.trigger_type = val; }
    float getTriggerWidth() const { return info.trigger_width; }
    void setTriggerWidth(float val) { info.trigger_width = val; }
    float getTriggerHeight() const { return info.trigger_height; }
    void setTriggerHeight(float val) { info.trigger_height = val; }
    float getTriggerRadius() const { return info.trigger_radius; }
    void setTriggerRadius(float val) { info.trigger_radius = val; }

    [[nodiscard]] b2Vec2 getPosition() const;
    [[nodiscard]] float getRotation() const;

    // Lua lifecycle functions
    void onStart();
    void onDestroy();

    // Lua physics methods
    void addForce(b2Vec2 force);
    void setVelocity(b2Vec2 vel);
    void setPosition(b2Vec2 pos);
    void setRotation(float degrees);
    void setAngularVelocity(float degrees);
    void setGravityScale(float scale);
    void setUpDirection(b2Vec2 direction);
    void setRightDirection(b2Vec2 direction);

    // Lua physics getters
    [[nodiscard]] b2Vec2 getVelocity() const;
    [[nodiscard]] float getAngularVelocity() const;
    [[nodiscard]] float getGravityScale() const;
    [[nodiscard]] b2Vec2 getUpDirection() const;
    [[nodiscard]] b2Vec2 getRightDirection() const;

    static void step();
};
