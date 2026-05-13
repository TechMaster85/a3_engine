#include "componentmanager.h"

#include "actor.h"
#include "core/fileutil.h"
#include "engine.h"
#include "particles/particlesystem.h"
#include "physics/rigidbody.h"

#include <lua/lua.hpp>

#include <LuaBridge/detail/LuaRef.h>

#include <filesystem>
#include <iostream>

namespace {
luabridge::LuaRef createLuaComponent(const char *componentTypeName) {
    luabridge::LuaRef typeTable =
        luabridge::getGlobal(Engine::L, componentTypeName);

    if (typeTable.isNil()) {
        const std::filesystem::path filePath =
            FileUtil::getComponentPath(componentTypeName);
        if (!std::filesystem::exists(filePath)) {
            std::cout << "error: failed to locate component "
                      << componentTypeName;
            exit(1);
        }

        if (luaL_dofile(Engine::L, filePath.string().c_str()) != LUA_OK) {
            std::cout << "problem with lua file " << componentTypeName;
            exit(1);
        }

        typeTable = luabridge::getGlobal(Engine::L, componentTypeName);
        typeTable["__index"] = typeTable;
    }

    luabridge::LuaRef newComponent = luabridge::newTable(Engine::L);
    luabridge::getGlobal(Engine::L, "setmetatable")(newComponent, typeTable);
    return newComponent;
}

luabridge::LuaRef createRigidbodyComponent(const RigidbodyProperties &rbInfo,
                                           const std::string &name,
                                           Actor *actor) {
    luabridge::LuaRef ref(Engine::L, Rigidbody{rbInfo, name, actor});
    return ref;
}

luabridge::LuaRef
createParticleSystemComponent(const ParticleSystemProperties &psp,
                              const std::string &name, Actor *actor) {
    luabridge::LuaRef ref(Engine::L, ParticleSystem{psp, name, actor});
    return ref;
}

} // namespace

luabridge::LuaRef
ComponentManager::createComponent(const std::string &type,
                                  const rapidjson::Value &propertiesJson,
                                  Actor *actor, const std::string &key) {
    if (type == "Rigidbody") {
        const RigidbodyProperties rbp =
            RigidbodyProperties::fromJson(propertiesJson);
        return createRigidbodyComponent(rbp, key, actor);
    }

    if (type == "ParticleSystem") {
        const ParticleSystemProperties psp =
            ParticleSystemProperties::fromJson(propertiesJson);
        return createParticleSystemComponent(psp, key, actor);
    }

    // Returns a fresh table equivalent to what's in component_type
    luabridge::LuaRef ref = createLuaComponent(type.c_str());

    ref["key"] = key;
    ref["type"] = type;
    ref["actor"] = actor;

    // Component should be enabled unless explicitly disabled
    if (ref["enabled"].isNil()) {
        ref["enabled"] = true;
    }

    // Insert overrides from the actor template
    for (const auto &prop : propertiesJson.GetObject()) {
        const std::string &propName = prop.name.GetString();
        if (prop.value.IsString()) {
            ref[propName] = prop.value.GetString();
        } else if (prop.value.IsInt()) {
            ref[propName] = prop.value.GetInt();
        } else if (prop.value.IsFloat()) {
            ref[propName] = prop.value.GetFloat();
        } else if (prop.value.IsBool()) {
            ref[propName] = prop.value.GetBool();
        }
    }

    return ref;
}
