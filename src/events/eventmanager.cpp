#include "eventmanager.h"

#include <algorithm>
#include <iostream>

void EventManager::publish(const std::string &eventType,
                           const luabridge::LuaRef &eventObject) {
    if (subscriptions.count(eventType) == 0) {
        return;
    }

    const auto &subscribers = subscriptions.at(eventType);
    for (const auto &sub : subscribers) {
        const auto result = sub.function(sub.component, eventObject);
        if (!result) {
            std::cout << result.message();
        }
    }
}

void EventManager::subscribe(const std::string &eventType,
                             const luabridge::LuaRef &component,
                             const luabridge::LuaRef &function) {
    pendingAdds.emplace_back(eventType, component, function);
}

void EventManager::unsubscribe(const std::string &eventType,
                               const luabridge::LuaRef &component,
                               const luabridge::LuaRef &function) {
    pendingRemoves.emplace_back(eventType, component, function);
}

void EventManager::updateSubscriptions() {
    // Apply removals
    for (const auto &removal : pendingRemoves) {
        if (subscriptions.count(removal.eventType) != 0) {
            auto &subs = subscriptions.at(removal.eventType);
            subs.erase(std::remove_if(subs.begin(), subs.end(),
                                      [&](const Subscription &s) {
                                          return s.component ==
                                                     removal.component &&
                                                 s.function == removal.function;
                                      }),
                       subs.end());
        }
    }
    pendingRemoves.clear();

    // Apply adds
    for (const auto &add : pendingAdds) {
        subscriptions[add.eventType].emplace_back(add.component, add.function);
    }
    pendingAdds.clear();
}
