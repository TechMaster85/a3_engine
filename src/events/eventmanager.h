#pragma once

#include "lua/lua.hpp"

#include "LuaBridge/LuaBridge.h"

#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

struct Subscription {
    luabridge::LuaRef component;
    luabridge::LuaRef function;

    Subscription(const luabridge::LuaRef &c, const luabridge::LuaRef &f)
        : component(c), function(f) {}
};

struct PendingChange {
    std::string eventType;
    luabridge::LuaRef component;
    luabridge::LuaRef function;

    PendingChange(std::string type, const luabridge::LuaRef &c,
                  const luabridge::LuaRef &f)
        : eventType(std::move(type)), component(c), function(f) {}
};

class EventManager {
public:
    static void publish(const std::string &eventType,
                        const luabridge::LuaRef &eventObject);
    static void subscribe(const std::string &eventType,
                          const luabridge::LuaRef &component,
                          const luabridge::LuaRef &function);
    static void unsubscribe(const std::string &eventType,
                            const luabridge::LuaRef &component,
                            const luabridge::LuaRef &function);

    static void updateSubscriptions();

private:
    static inline std::unordered_map<std::string, std::vector<Subscription>>
        subscriptions;
    static inline std::vector<PendingChange> pendingAdds;
    static inline std::vector<PendingChange> pendingRemoves;
};
