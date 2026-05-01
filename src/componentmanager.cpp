#include "componentmanager.h"

#include "actor.h"
#include "core/fileutil.h"
#include "particles/particlesystem.h"
#include "physics/rigidbody.h"

#include <LuaBridge/detail/LuaRef.h>
#include <filesystem>
#include <iostream>

namespace {
luabridge::LuaRef createLuaComponent(const std::string &componentTypeName) {
    luabridge::LuaRef typeTable =
        luabridge::getGlobal(ComponentManager::L, componentTypeName.c_str());

    if (typeTable.isNil()) {
        const std::filesystem::path filePath =
            FileUtil::getComponentPath(componentTypeName);
        if (!std::filesystem::exists(filePath)) {
            std::cout << "error: failed to locate component " +
                             componentTypeName;
            exit(1);
        }

        if (luaL_dofile(ComponentManager::L, filePath.string().c_str()) != LUA_OK) {
            std::cout << "problem with lua file " + componentTypeName;
            exit(1);
        }

        typeTable = luabridge::getGlobal(ComponentManager::L, componentTypeName.c_str());
        typeTable["__index"] = typeTable;
    }

    luabridge::LuaRef newComponent = luabridge::newTable(ComponentManager::L);

    newComponent.push(ComponentManager::L);
    typeTable.push(ComponentManager::L);
    lua_setmetatable(ComponentManager::L, -2);
    lua_pop(ComponentManager::L, 1);

    return newComponent;
}

luabridge::LuaRef createRigidbodyComponent(const RigidbodyProperties &rbInfo,
                                           const std::string &name,
                                           Actor *actor) {
    luabridge::LuaRef ref(ComponentManager::L, Rigidbody{rbInfo, name, actor});
    return ref;
}

luabridge::LuaRef createParticleSystemComponent(
    const ParticleSystemProperties &psp, const std::string &name,
    Actor *actor) {
    luabridge::LuaRef ref(ComponentManager::L, ParticleSystem{psp, name, actor});
    return ref;
}

} // namespace

ComponentManager::ComponentManager() {
    L = luaL_newstate();
    luaL_openlibs(L);
}

luabridge::LuaRef
ComponentManager::createComponent(const std::string &type,
                                  const rapidjson::Value &propertiesJson,
                                  Actor *actor, const std::string &key) {
    if (type == "Rigidbody") {
        const RigidbodyProperties rbp = RigidbodyProperties::fromJson(propertiesJson);
        return createRigidbodyComponent(rbp, key, actor);
    }

    if (type == "ParticleSystem") {
        const ParticleSystemProperties psp =
            ParticleSystemProperties::fromJson(propertiesJson);
        return createParticleSystemComponent(psp, key, actor);
    }

    // Returns a fresh table equivalent to what's in component_type
    luabridge::LuaRef ref = createLuaComponent(type);

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

luabridge::LuaRef ComponentManager::getGlobalRef() {
    return luabridge::LuaRef{L};
}
