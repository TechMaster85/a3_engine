#include "documentmanager.h"

#include "core/filepath.h"
#include "core/jsonutil.h"

#include <filesystem>
#include <iostream>
#include <utility>

const rapidjson::Document &DocumentManager::getScene(const std::string &name) {
    if (scenes.count(name) == 1) {
        return scenes.at(name);
    }

    const std::filesystem::path path = getScenePath(name);
    if (!std::filesystem::exists(path)) {
        std::cout << "error: scene " << name << " is missing";
        exit(1);
    }

    rapidjson::Document sceneDocument = loadJsonFile(path);
    scenes.emplace(name, std::move(sceneDocument));
    return scenes.at(name);
}

const rapidjson::Document &
DocumentManager::getTemplate(const std::string &name) {
    if (templates.count(name) == 1) {
        return templates.at(name);
    }

    const std::filesystem::path path = getTemplatePath(name);
    if (!std::filesystem::exists(path)) {
        std::cout << "error: template " << name << " is missing";
        exit(1);
    }

    rapidjson::Document templateDocument = loadJsonFile(path);
    templates.emplace(name, std::move(templateDocument));
    return templates.at(name);
}

const rapidjson::Document &DocumentManager::getGameConfig() {
    if (!gameConfig.IsNull()) {
        return gameConfig;
    }

    if (!std::filesystem::exists(GAME_CONFIG_PATH)) {
        std::cout << "error: " << RENDERING_CONFIG_PATH << " missing";
        exit(1);
    }

    gameConfig = loadJsonFile(GAME_CONFIG_PATH);
    return gameConfig;
}

const rapidjson::Document &DocumentManager::getRenderingConfig() {
    if (!renderingConfig.IsNull()) {
        return renderingConfig;
    }

    // Renderer has default values if config does not exist
    if (!std::filesystem::exists(RENDERING_CONFIG_PATH)) {
        renderingConfig.SetObject(); // Returns {} (empty document)
        return renderingConfig;
    }

    renderingConfig = loadJsonFile(RENDERING_CONFIG_PATH);
    return renderingConfig;
}
