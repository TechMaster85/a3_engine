#pragma once

#include "physics/collision.h"

#include <box2d/b2_contact.h>
#include <box2d/b2_math.h>
#include <box2d/b2_world_callbacks.h>

static const b2Vec2 CONTACT_LISTENER_SENTINEL = b2Vec2(-999.0F, -999.0F);

class ContactListener : public b2ContactListener {
    struct ContactData {
        b2Fixture *fixtureA = nullptr;
        b2Fixture *fixtureB = nullptr;
        Actor *actorA = nullptr;
        Actor *actorB = nullptr;
        Collision collisionA;
        Collision collisionB;
    };

    static ContactData buildContactData(b2Contact *contact) {
        ContactData d;
        d.fixtureA = contact->GetFixtureA();
        d.fixtureB = contact->GetFixtureB();
        d.actorA = static_cast<Actor *>(d.fixtureA->GetUserData().pointer);
        d.actorB = static_cast<Actor *>(d.fixtureB->GetUserData().pointer);
        const b2Vec2 relVel = d.fixtureA->GetBody()->GetLinearVelocity() -
                              d.fixtureB->GetBody()->GetLinearVelocity();
        d.collisionA.other = d.actorB;
        d.collisionA.relative_velocity = relVel;
        d.collisionB.other = d.actorA;
        d.collisionB.relative_velocity = relVel;
        return d;
    }

public:
    void BeginContact(b2Contact *contact) override {
        auto d = buildContactData(contact);

        if (!d.fixtureA->IsSensor() && !d.fixtureB->IsSensor()) {
            b2WorldManifold worldManifold;
            contact->GetWorldManifold(&worldManifold);
            d.collisionA.point = worldManifold.points[0];
            d.collisionA.normal = worldManifold.normal;
            d.collisionB.point = worldManifold.points[0];
            d.collisionB.normal = worldManifold.normal;
            ContactQueue::collisions.push_back({d.actorA, true, d.collisionA});
            ContactQueue::collisions.push_back({d.actorB, true, d.collisionB});
        }

        if (d.fixtureA->IsSensor() && d.fixtureB->IsSensor()) {
            d.collisionA.point = CONTACT_LISTENER_SENTINEL;
            d.collisionA.normal = CONTACT_LISTENER_SENTINEL;
            d.collisionB.point = CONTACT_LISTENER_SENTINEL;
            d.collisionB.normal = CONTACT_LISTENER_SENTINEL;
            ContactQueue::triggers.push_back({d.actorA, true, d.collisionA});
            ContactQueue::triggers.push_back({d.actorB, true, d.collisionB});
        }
    }

    void EndContact(b2Contact *contact) override {
        auto d = buildContactData(contact);
        d.collisionA.point = CONTACT_LISTENER_SENTINEL;
        d.collisionA.normal = CONTACT_LISTENER_SENTINEL;
        d.collisionB.point = CONTACT_LISTENER_SENTINEL;
        d.collisionB.normal = CONTACT_LISTENER_SENTINEL;

        if (!d.fixtureA->IsSensor() && !d.fixtureB->IsSensor()) {
            ContactQueue::collisions.push_back({d.actorA, false, d.collisionA});
            ContactQueue::collisions.push_back({d.actorB, false, d.collisionB});
        }

        if (d.fixtureA->IsSensor() && d.fixtureB->IsSensor()) {
            ContactQueue::triggers.push_back({d.actorA, false, d.collisionA});
            ContactQueue::triggers.push_back({d.actorB, false, d.collisionB});
        }
    }
};
