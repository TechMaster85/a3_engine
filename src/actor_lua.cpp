#include "actor.h"

#include "componentmanager.h"

#include <string>

const std::string &Actor::getName() const { return name; }
size_t Actor::getId() const { return id; }

luabridge::LuaRef Actor::getComponentByKey(const std::string &key) const {
    if (components.count(key) != 0) {
        return components.at(key).ref;
    }
    if (pendingComponents.count(key) != 0) {
        return pendingComponents.at(key).ref;
    }
    return ComponentManager::getGlobalRef();
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
    return ComponentManager::getGlobalRef();
}

luabridge::LuaRef Actor::getComponents(const std::string &typeName) const {
    luabridge::LuaRef componentsTable =
        luabridge::newTable(ComponentManager::getGlobalRef());
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
    luabridge::LuaRef ref = ComponentManager::createComponent(typeName, noProperties, this, key);

    pendingComponents.emplace(key, ComponentRef(ref));

    return ref;
}

void Actor::removeComponent(const luabridge::LuaRef &componentRef) {
    const std::string &key = componentRef["key"].tostring();
    componentsToRemoval.emplace_back(key);
}
