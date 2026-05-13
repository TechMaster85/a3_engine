#include "documentmanager.h"

#include "core/fileutil.h"
#include "core/jsonutil.h"

#include <filesystem>
#include <iostream>
#include <utility>

const rapidjson::Document &DocumentManager::getScene(const std::string &name) {
    if (scenes.count(name) == 1) {
        return scenes.at(name);
    }

    const std::filesystem::path path = FileUtil::getScenePath(name);
    if (!std::filesystem::exists(path)) {
        std::cout << "error: scene " << name << " is missing";
        exit(1);
    }

    rapidjson::Document sceneDocument = JsonUtil::loadJsonFile(path);
    scenes.emplace(name, std::move(sceneDocument));
    return scenes.at(name);
}

const rapidjson::Document &
DocumentManager::getTemplate(const std::string &name) {
    if (templates.count(name) == 1) {
        return templates.at(name);
    }

    const std::filesystem::path path = FileUtil::getTemplatePath(name);
    if (!std::filesystem::exists(path)) {
        std::cout << "error: template " << name << " is missing";
        exit(1);
    }

    rapidjson::Document templateDocument = JsonUtil::loadJsonFile(path);
    templates.emplace(name, std::move(templateDocument));
    return templates.at(name);
}
