#pragma once

#include "actor.h"

#include <memory>
#include <string>
#include <vector>

class SceneDB {
private:
    static inline std::vector<std::unique_ptr<Actor>> actors;
    static inline std::vector<std::unique_ptr<Actor>> pendingAddedActors;

    static inline std::string currentSceneName;
    static inline std::string nextSceneName;

public:
    explicit SceneDB() = default;

    // Lua APIs for all actors
    static void loadScene(const std::string &sceneName);
    static Actor *findActor(const std::string &name);
    static luabridge::LuaRef findAllActors(const std::string &name);
    static Actor *instantiateActor(const std::string &actorTemplateName);
    static void destroyActor(Actor *actor);
    static void load(const std::string &sceneName);
    static const std::string &getCurrentScene();
    static void dontDestroy(Actor *actor);

    static void update(); // OnUpdate
    static void lateUpdate(); // Physics dispatch + OnLateUpdate + Destruction
    static void syncPending();
    static void loadSceneOnUpdate(const std::string &sceneName);
    static void processDestruction();
};
