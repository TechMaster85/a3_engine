#pragma once

#include <lua/lua.hpp>

#include <LuaBridge/LuaBridge.h>

#include <rapidjson/document.h>

struct Actor;
struct RigidbodyProperties;
struct ParticleSystemProperties;

class ComponentManager {
public:
    explicit ComponentManager(lua_State *luaState) { L = luaState; }

    static luabridge::LuaRef
    createComponent(const std::string &type,
                    const rapidjson::Value &propertiesJson, Actor *actor,
                    const std::string &key);

private:
    static inline lua_State *L = nullptr;

    static luabridge::LuaRef createLuaComponent(const char *componentTypeName);
    static luabridge::LuaRef
    createRigidbodyComponent(const RigidbodyProperties &rbInfo,
                             const std::string &name, Actor *actor);
    static luabridge::LuaRef
    createParticleSystemComponent(const ParticleSystemProperties &psp,
                                  const std::string &name, Actor *actor);
};
