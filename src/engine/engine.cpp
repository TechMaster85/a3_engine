#include "engine.h"

#include "audio/audio.h"
#include "core/config.h"
#include "events/eventmanager.h"
#include "input/input.h"
#include "physics/rigidbody.h"
#include "rendering/renderer.h"
#include "rendering/textdb.h"
#include "scene/componentmanager.h"
#include "scene/documentmanager.h"
#include "scene/scenedb.h"
#include "scripting/luabindings.h"

#include <lua/lua.hpp>

#include <cassert>
#include <iostream>
#include <thread>
#ifdef __SWITCH__
#include <switch.h>
#endif

void Engine::startup() {
    std::ios_base::sync_with_stdio(false);

#ifdef __SWITCH__
    romfsInit();
#endif
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER);
    ComponentManager();
    DocumentManager();
    Audio();
    Config();
    InputState();
    Renderer();
    SceneDB();
    TextDB();

    L = luaL_newstate();
    luaL_openlibs(L);
    LuaBindings::registerAll(L);

    SceneDB::loadScene(Config::initialScene);
#ifdef __SWITCH__
    Input::initializeController();
#endif
}

void Engine::input() {
    InputState::resetFrame();
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
        if (e.type == SDL_QUIT) {
            Engine::running = false;
            break;
        }
        InputState::handleEvent(e);
    }
}

void Engine::loop() {
#ifdef __SWITCH__
    while (running && appletMainLoop()) {
#else
    while (running) {
#endif
        Engine::input();
        SceneDB::update();
        SceneDB::lateUpdate();
        EventManager::updateSubscriptions();
        Rigidbody::step();
        Renderer::update();
        SceneDB::syncPending();
        ++frameNumber;
    }
}

void Engine::sleep(int x) {
    std::this_thread::sleep_for(std::chrono::milliseconds(x));
}

void Engine::openUrl(const std::string &url) {
#ifdef _WIN32
    const std::string command = "start " + url;
#elif defined(__APPLE__)
    const std::string command = "open " + url;
#else
    const std::string command = "xdg-open " + url;
#endif
    int result = std::system(command.c_str());
    if (result == -1) {
        return;
    }
}
