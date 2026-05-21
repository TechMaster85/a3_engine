#include "config.h"

#include "core/fileutil.h"
#include "core/jsonutil.h"

#include <rapidjson/document.h>

#include <iostream>
#include <string>

Config::Config() {
    if (!std::filesystem::exists(FileUtil::GAME_CONFIG_PATH)) {
        std::cout << "error: " << FileUtil::GAME_CONFIG_PATH << " missing";
        exit(1);
    }

    const rapidjson::Document gameConfigDocument =
        JsonUtil::loadJsonFile(FileUtil::GAME_CONFIG_PATH);
    JsonUtil::get(gameConfigDocument, "game_title", gameTitle);
    JsonUtil::get(gameConfigDocument, "initial_scene", initialScene);

    if (initialScene.empty()) {
        std::cout << "error: initial_scene unspecified";
        exit(1);
    }
}
