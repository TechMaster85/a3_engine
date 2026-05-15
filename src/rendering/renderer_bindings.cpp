#include "renderer.h"

#include <lua/lua.hpp>

#include <LuaBridge/LuaBridge.h>

void Renderer::registerLuaBindings(lua_State *L) {

    luabridge::getGlobalNamespace(L)
        .beginNamespace("Text")
        .addFunction("Draw", Renderer::drawText)
        .endNamespace();

    luabridge::getGlobalNamespace(L)
        .beginNamespace("Image")
        .addFunction("Draw", Renderer::draw)
        .addFunction("DrawEx", Renderer::drawEx)
        .addFunction("DrawUI", Renderer::drawUi)
        .addFunction("DrawUIEx", Renderer::drawUiEx)
        .addFunction("DrawPixel", Renderer::drawPixel)
        .endNamespace();

    luabridge::getGlobalNamespace(L)
        .beginNamespace("Camera")
        .addFunction("SetPosition", Renderer::setPosition)
        .addFunction("GetPositionX", Renderer::getPositionX)
        .addFunction("GetPositionY", Renderer::getPositionY)
        .addFunction("SetZoom", Renderer::setZoom)
        .addFunction("GetZoom", Renderer::getZoom)
        .endNamespace();
}
