#include "scenedb.h"

#include <algorithm>
#include <cstring>
#include <memory>
#include <string>

#include "actor.h"
#include "componentmanager.h"
#include "documentmanager.h"

namespace {
void addPendingActors(std::vector<std::unique_ptr<Actor>> &pendingAddedActors,
                      std::vector<std::unique_ptr<Actor>> &actors) {
    std::vector<std::unique_ptr<Actor>> toProcess;
    std::swap(toProcess, pendingAddedActors);

    const size_t startIndex = actors.size();
    for (auto &actor : toProcess) {
        if (actor && !actor->destroyed) {
            actors.emplace_back(std::move(actor));
        }
    }
    for (size_t i = startIndex; i < actors.size(); ++i) {
        actors[i]->start();
    }
}
} // namespace

void SceneDB::loadScene(const std::string &sceneName) {
    const rapidjson::Document &sceneDocument =
        DocumentManager::getScene(sceneName);
    if (!sceneDocument.HasMember("actors")) {
        return;
    }
    const rapidjson::Value &actorObjects = sceneDocument["actors"];
    currentSceneName = sceneName;

    // Call OnDestroy on actors being removed by scene load
    for (const auto &actor : actors) {
        if (!actor->dontDestroy) {
            actor->callOnDestroy();
        }
    }

    actors.erase(
        std::remove_if(actors.begin(), actors.end(),
                       [](const auto &actor) { return !actor->dontDestroy; }),
        actors.end());

    const size_t numActors = actorObjects.Size();
    const size_t survivorCount = actors.size();
    actors.reserve(survivorCount + numActors);

    for (const auto &actorJson : actorObjects.GetArray()) {
        auto actor = std::make_unique<Actor>();
        actor->id = Actor::nextActorId++;
        actor->build(actorJson);
        actors.emplace_back(std::move(actor));
    }

    for (size_t i = survivorCount; i < actors.size(); ++i) {
        actors[i]->start();
    }
}

Actor *SceneDB::findActor(const std::string &name) {
    for (const auto &actor : actors) {
        if (actor->getName() == name && !actor->destroyed) {
            return actor.get();
        }
    }
    for (const auto &actor : pendingAddedActors) {
        if (actor->getName() == name && !actor->destroyed) {
            return actor.get();
        }
    }
    return nullptr;
}

luabridge::LuaRef SceneDB::findAllActors(const std::string &name) {
    luabridge::LuaRef actorsTable =
        luabridge::newTable(ComponentManager::getGlobalRef());
    int actorsFound = 0;
    for (const auto &actor : actors) {
        if (actor->getName() == name && !actor->destroyed) {
            actorsTable[++actorsFound] = actor.get();
        }
    }
    for (const auto &actor : pendingAddedActors) {
        if (actor->getName() == name && !actor->destroyed) {
            actorsTable[++actorsFound] = actor.get();
        }
    }
    return actorsTable;
}

Actor *SceneDB::instantiateActor(const std::string &actorTemplateName) {
    auto actor = std::make_unique<Actor>();
    actor->id = Actor::nextActorId++;

    const auto &templateDoc = DocumentManager::getTemplate(actorTemplateName);
    actor->build(templateDoc);

    Actor *actorPtr = actor.get();
    pendingAddedActors.emplace_back(std::move(actor));
    return actorPtr;
}

void SceneDB::destroyActor(Actor *actor) {
    if (actor != nullptr) {
        actor->destroyed = true;
    }
}

void SceneDB::load(const std::string &sceneName) {
    loadSceneOnUpdate(sceneName);
}

const std::string &SceneDB::getCurrentScene() { return currentSceneName; }

void SceneDB::processDestruction() {
    for (const auto &actor : actors) {
        if (actor->destroyed) {
            actor->callOnDestroy();
        }
    }
    actors.erase(std::remove_if(actors.begin(), actors.end(),
                                [](const auto &actor) {
                                    return actor->destroyed;
                                }),
                 actors.end());
}

void SceneDB::loadSceneOnUpdate(const std::string &sceneName) {
    nextSceneName = sceneName;
}

void SceneDB::syncPending() {
    addPendingActors(pendingAddedActors, actors);

    for (const auto &actor : actors) {
        std::map<std::string, ComponentRef> pending;
        std::swap(pending, actor->pendingComponents);
        for (const auto &[key, component] : pending) {
            actor->components.emplace(key, component);
            if (component.hasOnStart) {
                component.call("OnStart");
            }
        }
    }
}

void SceneDB::update() {
    if (!nextSceneName.empty()) {
        loadScene(nextSceneName);
        nextSceneName = "";
    }

    for (const auto &actor : actors) {
        for (const auto &[key, component] : actor->components) {
            if (actor->destroyed) {
                break;
            }
            if (component.hasOnUpdate) {
                component.call("OnUpdate");
            }
        }
    }
}

void SceneDB::lateUpdate() {
    for (const auto &actor : actors) {
        for (const auto &[key, component] : actor->components) {
            if (actor->destroyed) {
                break;
            }
            if (component.hasOnLateUpdate) {
                component.call("OnLateUpdate");
            }
        }
    }

    // Call OnDestroy for components being removed individually via
    // self.actor:RemoveComponent() This happens after LateUpdate, but before
    // the actor itself is destroyed.
    for (const auto &actor : actors) {
        if (!actor->componentsToRemoval.empty()) {
            std::sort(actor->componentsToRemoval.begin(),
                      actor->componentsToRemoval.end());
            for (const auto &key : actor->componentsToRemoval) {
                if (actor->components.count(key) != 0) {
                    ComponentRef &component = actor->components.at(key);
                    if (component.hasOnDestroy) {
                        component.callOnDestroy();
                    }
                    actor->components.erase(key);
                }
            }
            actor->componentsToRemoval.clear();
        }
    }

    processDestruction();
}

void SceneDB::dontDestroy(Actor *actor) { actor->dontDestroy = true; }
