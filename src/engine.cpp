#include "engine.h"

#include "Helper.h"

#include "audio/audio.h"
#include "componentmanager.h"
#include "config.h"
#include "documentmanager.h"
#include "events/eventmanager.h"
#include "input/input.h"
#include "luabindings.h"
#include "physics/rigidbody.h"
#include "rendering/renderer.h"
#include "rendering/textdb.h"
#include "scenedb.h"

#include <cassert>
#include <iostream>
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
    LuaBindings{ComponentManager::L};
    Input();
    Renderer();
    SceneDB();
    TextDB();
    SceneDB::loadScene(Config::initialScene);
#ifdef __SWITCH__
    Input::initializeController();
#endif
}

void Engine::input() {
    Input::resetFrame();
    SDL_Event e;
    while (Helper::SDL_PollEvent(&e) != 0) {
        if (e.type == SDL_QUIT) {
            Engine::running = false;
            break;
        }
        Input::handleEvent(e);
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
    }
}
