#include "particlesystem.h"

#include <lua/lua.hpp>

#include <LuaBridge/LuaBridge.h>

void ParticleSystem::registerLuaBindings(lua_State *L) {
    luabridge::getGlobalNamespace(L)
        .beginClass<ParticleSystem>("ParticleSystem")
        .addFunction("OnUpdate", &ParticleSystem::onUpdate)
        .addFunction("Stop", &ParticleSystem::stop)
        .addFunction("Play", &ParticleSystem::play)
        .addFunction("Burst", &ParticleSystem::burst)
        .addProperty("enabled", &ParticleSystem::enabled)
        .addProperty("type", &ParticleSystem::getType)
        .addProperty("x", &ParticleSystem::getX, &ParticleSystem::setX)
        .addProperty("y", &ParticleSystem::getY, &ParticleSystem::setY)
        .addProperty("start_color_r", &ParticleSystem::getStartColorR,
                     &ParticleSystem::setStartColorR)
        .addProperty("start_color_g", &ParticleSystem::getStartColorG,
                     &ParticleSystem::setStartColorG)
        .addProperty("start_color_b", &ParticleSystem::getStartColorB,
                     &ParticleSystem::setStartColorB)
        .endClass();
}
