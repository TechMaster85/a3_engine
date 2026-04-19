#include "rigidbody.h"

#include "box2d/b2_circle_shape.h"
#include "box2d/b2_fixture.h"
#include "box2d/b2_polygon_shape.h"
#include "physics/collision.h"

#include "actor.h"
#include "physics/contact_listener.h"

RigidbodyProperties RigidbodyProperties::fromJson(const rapidjson::Value &json) {
    RigidbodyProperties rbInfo;
    for (const auto &prop : json.GetObject()) {
        const std::string &propName = prop.name.GetString();
        if (propName == "x") {
            rbInfo.x = prop.value.GetFloat();
        } else if (propName == "y") {
            rbInfo.y = prop.value.GetFloat();
        } else if (propName == "body_type") {
            rbInfo.body_type = prop.value.GetString();
        } else if (propName == "precise") {
            rbInfo.precise = prop.value.GetBool();
        } else if (propName == "gravity_scale") {
            rbInfo.gravity_scale = prop.value.GetFloat();
        } else if (propName == "density") {
            rbInfo.density = prop.value.GetFloat();
        } else if (propName == "angular_friction") {
            rbInfo.angular_friction = prop.value.GetFloat();
        } else if (propName == "rotation") {
            rbInfo.rotation = prop.value.GetFloat();
        } else if (propName == "has_collider") {
            rbInfo.has_collider = prop.value.GetBool();
        } else if (propName == "has_trigger") {
            rbInfo.has_trigger = prop.value.GetBool();
        } else if (propName == "collider_type") {
            rbInfo.collider_type = prop.value.GetString();
        } else if (propName == "width") {
            rbInfo.width = prop.value.GetFloat();
        } else if (propName == "height") {
            rbInfo.height = prop.value.GetFloat();
        } else if (propName == "radius") {
            rbInfo.radius = prop.value.GetFloat();
        } else if (propName == "friction") {
            rbInfo.friction = prop.value.GetFloat();
        } else if (propName == "bounciness") {
            rbInfo.bounciness = prop.value.GetFloat();
        } else if (propName == "trigger_type") {
            rbInfo.trigger_type = prop.value.GetString();
        } else if (propName == "trigger_width") {
            rbInfo.trigger_width = prop.value.GetFloat();
        } else if (propName == "trigger_height") {
            rbInfo.trigger_height = prop.value.GetFloat();
        } else if (propName == "trigger_radius") {
            rbInfo.trigger_radius = prop.value.GetFloat();
        }
    }
    return rbInfo;
}

void Rigidbody::registerContactListener() {
    static ContactListener listener;
    static bool registered = false;
    if (!registered) {
        getWorld().SetContactListener(&listener);
        registered = true;
    }
}

b2World &Rigidbody::getWorld() {
    static b2World instance(b2Vec2(0.0F, GRAVITY));
    return instance;
}

Rigidbody::Rigidbody(RigidbodyProperties info, std::string key, Actor *actor)
    : key(std::move(key)), actor(actor), info(std::move(info)) {}

void Rigidbody::onStart() {
    registerContactListener();

    b2BodyDef bodyDef;
    bodyDef.position = {info.x, info.y};
    bodyDef.bullet = info.precise;
    bodyDef.gravityScale = info.gravity_scale;
    bodyDef.angularDamping = info.angular_friction;
    bodyDef.angle = degToRad(info.rotation);

    if (info.body_type == "static") {
        bodyDef.type = b2_staticBody;
    } else if (info.body_type == "kinematic") {
        bodyDef.type = b2_kinematicBody;
    } else {
        bodyDef.type = b2_dynamicBody;
    }

    body = getWorld().CreateBody(&bodyDef);

    if (!info.has_collider && !info.has_trigger) {
        b2PolygonShape shape;
        shape.SetAsBox(info.width * 0.5F, info.height * 0.5F);
        b2FixtureDef fixtureDef;
        fixtureDef.shape = &shape;
        fixtureDef.density = info.density;
        fixtureDef.isSensor = true;
        fixtureDef.filter.categoryBits = 0x0002;
        fixtureDef.filter.maskBits = 0x0002;
        body->CreateFixture(&fixtureDef);
    } else {
        if (info.has_collider) {
            b2FixtureDef fixtureDef;
            fixtureDef.density = info.density;
            fixtureDef.friction = info.friction;
            fixtureDef.restitution = info.bounciness;
            fixtureDef.filter.categoryBits = 0x0001;
            fixtureDef.filter.maskBits = 0x0001;
            fixtureDef.userData.pointer = reinterpret_cast<uintptr_t>(actor);
            b2PolygonShape box;
            b2CircleShape circle;
            if (info.collider_type == "circle") {
                circle.m_radius = info.radius;
                fixtureDef.shape = &circle;
            } else {
                box.SetAsBox(info.width * 0.5F, info.height * 0.5F);
                fixtureDef.shape = &box;
            }
            body->CreateFixture(&fixtureDef);
        }
        if (info.has_trigger) {
            b2FixtureDef fixtureDef;
            fixtureDef.density = info.density;
            fixtureDef.friction = info.friction;
            fixtureDef.restitution = info.bounciness;
            fixtureDef.isSensor = true;
            fixtureDef.filter.categoryBits = 0x0002;
            fixtureDef.filter.maskBits = 0x0002;
            fixtureDef.userData.pointer = reinterpret_cast<uintptr_t>(actor);
            b2PolygonShape box;
            b2CircleShape circle;
            if (info.trigger_type == "circle") {
                circle.m_radius = info.trigger_radius;
                fixtureDef.shape = &circle;
            } else {
                box.SetAsBox(info.trigger_width * 0.5F,
                             info.trigger_height * 0.5F);
                fixtureDef.shape = &box;
            }
            body->CreateFixture(&fixtureDef);
        }
    }
}

void Rigidbody::onDestroy() {
    if (body != nullptr) {
        getWorld().DestroyBody(body);
        body = nullptr;
    }
}

b2Vec2 Rigidbody::getPosition() const {
    if (body == nullptr) {
        return b2Vec2(info.x, info.y);
    }
    return body->GetPosition();
}

float Rigidbody::getRotation() const {
    if (body == nullptr) {
        return info.rotation;
    }
    return radToDeg(body->GetAngle());
}

void Rigidbody::addForce(b2Vec2 force) {
    if (body != nullptr) {
        body->ApplyForceToCenter(force, true);
    }
}

void Rigidbody::setVelocity(b2Vec2 vel) {
    if (body != nullptr) {
        body->SetLinearVelocity(vel);
    }
}

void Rigidbody::setPosition(b2Vec2 pos) {
    if (body == nullptr) {
        info.x = pos.x;
        info.y = pos.y;
    } else {
        body->SetTransform(pos, body->GetAngle());
    }
}

void Rigidbody::setRotation(float degrees) {
    if (body == nullptr) {
        info.rotation = degrees;
    } else {
        body->SetTransform(body->GetPosition(), degToRad(degrees));
    }
}

void Rigidbody::setAngularVelocity(float degrees) {
    if (body != nullptr) {
        body->SetAngularVelocity(degToRad(degrees));
    }
}

void Rigidbody::setGravityScale(float scale) {
    info.gravity_scale = scale;
    if (body != nullptr) {
        body->SetGravityScale(scale);
    }
}

void Rigidbody::setUpDirection(b2Vec2 direction) {
    if (body == nullptr) {
        return;
    }
    direction.Normalize();
    body->SetTransform(body->GetPosition(),
                       glm::atan(direction.x, -direction.y));
}

void Rigidbody::setRightDirection(b2Vec2 direction) {
    if (body == nullptr) {
        return;
    }
    direction.Normalize();
    body->SetTransform(body->GetPosition(),
                       glm::atan(direction.x, -direction.y) - (b2_pi / 2.0F));
}

b2Vec2 Rigidbody::getVelocity() const {
    return (body != nullptr) ? body->GetLinearVelocity() : b2Vec2(0, 0);
}

float Rigidbody::getAngularVelocity() const {
    return (body != nullptr) ? radToDeg(body->GetAngularVelocity()) : 0.0F;
}

float Rigidbody::getGravityScale() const {
    return (body != nullptr) ? body->GetGravityScale() : info.gravity_scale;
}

b2Vec2 Rigidbody::getUpDirection() const {
    if (body == nullptr) {
        return b2Vec2(0.0F, -1.0F);
    }
    const float angle = body->GetAngle();
    return b2Vec2(glm::sin(angle), -glm::cos(angle));
}

b2Vec2 Rigidbody::getRightDirection() const {
    if (body == nullptr) {
        return b2Vec2(1.0F, 0.0F);
    }
    const float angle = body->GetAngle();
    return b2Vec2(glm::cos(angle), glm::sin(angle));
}

void Rigidbody::step() {
    getWorld().Step(1.0F / 60.0F, 8, 3);

    for (const auto &event : ContactQueue::collisions) {
        if ((event.receiver != nullptr) && (event.collision.other != nullptr)) {
            if (event.isEnter) {
                event.receiver->enterCollision(event.collision);
            } else {
                event.receiver->exitCollision(event.collision);
            }
        }
    }
    ContactQueue::collisions.clear();

    for (const auto &event : ContactQueue::triggers) {
        if ((event.receiver != nullptr) && (event.collision.other != nullptr)) {
            if (event.isEnter) {
                event.receiver->enterTrigger(event.collision);
            } else {
                event.receiver->exitTrigger(event.collision);
            }
        }
    }
    ContactQueue::triggers.clear();
}
