#include "actor.h"

#include "componentmanager.h"
#include "documentmanager.h"
#include "engine/engine.h"

#include <lua/lua.hpp>

#include <LuaBridge/LuaBridge.h>

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
    const luabridge::LuaRef ref = ComponentManager::createComponent(
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

const std::string &Actor::getName() const { return name; }
size_t Actor::getId() const { return id; }

luabridge::LuaRef Actor::getComponentByKey(const std::string &key) const {
    if (components.count(key) != 0) {
        return components.at(key).ref;
    }
    if (pendingComponents.count(key) != 0) {
        return pendingComponents.at(key).ref;
    }
    return Engine::L;
}

luabridge::LuaRef Actor::getComponent(const std::string &typeName) const {
    for (const auto &[key, component] : components) {
        if (component.ref["type"] == typeName) {
            return component.ref;
        }
    }
    for (const auto &[key, component] : pendingComponents) {
        if (component.ref["type"] == typeName) {
            return component.ref;
        }
    }
    return Engine::L;
}

luabridge::LuaRef Actor::getComponents(const std::string &typeName) const {
    const luabridge::LuaRef componentsTable = luabridge::newTable(Engine::L);
    int componentsFound = 0;
    for (const auto &[key, component] : components) {
        if (component.ref["type"] == typeName) {
            componentsTable[++componentsFound] = component.ref;
        }
    }
    for (const auto &[key, component] : pendingComponents) {
        if (component.ref["type"] == typeName) {
            componentsTable[++componentsFound] = component.ref;
        }
    }
    return componentsTable;
}

luabridge::LuaRef Actor::addComponent(const std::string &typeName) {
    const size_t n = componentsAddedDuringRuntime++;
    const std::string key = "r" + std::to_string(n);

    rapidjson::Value noProperties;
    noProperties.SetObject();
    luabridge::LuaRef ref =
        ComponentManager::createComponent(typeName, noProperties, this, key);

    pendingComponents.emplace(key, ComponentRef(ref));

    return ref;
}

void Actor::removeComponent(const luabridge::LuaRef &componentRef) {
    const std::string &key = componentRef["key"].tostring();
    componentsToRemoval.emplace_back(key);
}
