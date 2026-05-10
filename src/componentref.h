#pragma once

#include "lua/lua.hpp"

#include "LuaBridge/LuaBridge.h"

#include <utility>

struct ComponentRef {
    ComponentRef(luabridge::LuaRef r);
    luabridge::LuaRef ref;

    bool hasOnStart = false;
    bool hasOnUpdate = false;
    bool hasOnLateUpdate = false;
    bool hasOnCollisionEnter = false;
    bool hasOnCollisionExit = false;
    bool hasOnTriggerEnter = false;
    bool hasOnTriggerExit = false;
    bool hasOnDestroy = false;

    [[nodiscard]] bool isEnabled() const { return ref["enabled"]; }
    void enable() const { ref["enabled"] = true; }
    void disable() const { ref["enabled"] = false; }

    // Calls fn(self, args...) if enabled. Catches and reports Lua errors.
    template <typename... Args>
    void callWith(const std::string &functionName, Args &&...args) const {
        if (!isEnabled()) {
            return;
        }
        const auto result = ref[functionName](ref, std::forward<Args>(args)...);
        if (!result) {
            reportError(result.message());
        }
    }

    // Convenience wrapper for the no-argument case.
    void call(const std::string &functionName) const {
        callWith(functionName);
    }

    // Called regardless of enabled state, as per spec.
    void callOnDestroy() const {
        const auto result = ref["OnDestroy"](ref);
        if (!result) {
            reportError(result.message());
        }
    }

private:
    void reportError(const std::string &error) const;
};
