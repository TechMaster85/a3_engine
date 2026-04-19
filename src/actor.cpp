#include "actor.h"

#include "componentmanager.h"
#include "documentmanager.h"

#include <cassert>

void Actor::build(const rapidjson::Value &actorJson) {
    if (actorJson.HasMember("template")) {
        build(DocumentManager::getTemplate(actorJson["template"].GetString()));
    }
    if (actorJson.HasMember("name")) {
        name = actorJson["name"].GetString();
    }
    if (!actorJson.HasMember("components")) {
        return;
    }
    for (const auto &component : actorJson["components"].GetObject()) {
        const std::string &key = component.name.GetString();
        const rapidjson::Value &propertiesJson = component.value;
        loadComponent(key, propertiesJson);
    }
}

void Actor::loadComponent(const std::string &key,
                          const rapidjson::Value &propertiesJson) {
    luabridge::LuaRef ref = ComponentManager::createComponent(
        propertiesJson["type"].GetString(), propertiesJson, this, key);

    components.emplace(key, ComponentRef(ref));
}

void Actor::enterCollision(const Collision &collision) {
    for (const auto &[key, component] : components) {
        if (component.hasOnCollisionEnter) {
            component.callWith("OnCollisionEnter", collision);
        }
    }
}

void Actor::exitCollision(const Collision &collision) {
    for (const auto &[key, component] : components) {
        if (component.hasOnCollisionExit) {
            component.callWith("OnCollisionExit", collision);
        }
    }
}

void Actor::enterTrigger(const Collision &collision) {
    for (const auto &[key, component] : components) {
        if (component.hasOnTriggerEnter) {
            component.callWith("OnTriggerEnter", collision);
        }
    }
}

void Actor::exitTrigger(const Collision &collision) {
    for (const auto &[key, component] : components) {
        if (component.hasOnTriggerExit) {
            component.callWith("OnTriggerExit", collision);
        }
    }
}

void Actor::destroy() { destroyed = true; }

void Actor::callOnDestroy() {
    for (const auto &[key, component] : components) {
        if (component.hasOnDestroy) {
            component.callOnDestroy();
        }
    }
}

void Actor::start() {
    for (const auto &[key, component] : components) {
        if (component.hasOnStart) {
            component.call("OnStart");
        }
    }
}
