#pragma once

#include "componentref.h"

#include "physics/collision.h"
#include "rapidjson/document.h"

#include <map>
#include <string>

struct Actor {
    size_t id;
    std::string name;
    std::map<std::string, ComponentRef> components;
    std::map<std::string, ComponentRef> pendingComponents;
    std::vector<std::string> componentsToRemoval;
    bool dontDestroy = false;
    bool destroyed = false;

    // Lua bindings
    [[nodiscard]] const std::string &getName() const;
    [[nodiscard]] size_t getId() const;
    [[nodiscard]] luabridge::LuaRef
    getComponentByKey(const std::string &key) const;
    [[nodiscard]] luabridge::LuaRef
    getComponent(const std::string &typeName) const;
    [[nodiscard]] luabridge::LuaRef
    getComponents(const std::string &typeName) const;
    luabridge::LuaRef addComponent(const std::string &typeName);
    void removeComponent(const luabridge::LuaRef &componentRef);

    // Helper functions for scenedb
    void build(const rapidjson::Value &actorJson);
    void loadComponent(const std::string &key,
                       const rapidjson::Value &propertiesJson);
    void enterCollision(const Collision &collision);
    void exitCollision(const Collision &collision);
    void enterTrigger(const Collision &collision);
    void exitTrigger(const Collision &collision);
    void destroy();
    void callOnDestroy();
    void start();

    static inline size_t nextActorId;
    static inline size_t componentsAddedDuringRuntime = 0;
};
