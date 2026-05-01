#include "config.h"

#include "documentmanager.h"
#include "core/fileutil.h"
#include "rapidjson/document.h"

#include <iostream>
#include <string>

Config::Config() {
    if (!std::filesystem::exists(FileUtil::GAME_CONFIG_PATH)) {
        std::cout << "error: " << FileUtil::GAME_CONFIG_PATH << " missing";
        exit(1);
    }

    const rapidjson::Document &gameConfigDocument =
        DocumentManager::getGameConfig();

    for (const auto &member : gameConfigDocument.GetObject()) {
        const std::string_view key = member.name.GetString();

        if (key == "game_title") {
            gameTitle = member.value.GetString();
        } else if (key == "initial_scene") {
            initialScene = member.value.GetString();
        }
    }

    if (initialScene.empty()) {
        std::cout << "error: initial_scene unspecified";
        exit(1);
    }
}
