#include "documentmanager.h"

#include "filepath.h"
#include "rapidjson/filereadstream.h"

#include <filesystem>
#include <iostream>
#include <utility>

namespace {
void loadFile(const std::filesystem::path &path,
              rapidjson::Document &outDocument) {
    const int MAX_BUFFER = 65536;
    FILE *file_pointer = nullptr;
#ifdef _WIN32
    fopen_s(&file_pointer, path.string().c_str(), "rb");
#else
    file_pointer = fopen(path.string().c_str(), "rb");
#endif
    assert(file_pointer != nullptr);
    char buffer[MAX_BUFFER];
    rapidjson::FileReadStream stream(file_pointer, buffer, sizeof(buffer));
    outDocument.ParseStream(stream);
    std::fclose(file_pointer);

    if (outDocument.HasParseError()) {
        rapidjson::ParseErrorCode errorCode = outDocument.GetParseError();
        std::cout << "error parsing json at [" << path << "]" << '\n';
        std::cout << "error code " << errorCode << '\n';
        exit(0);
    }
}
} // namespace

const rapidjson::Document &DocumentManager::getScene(const std::string &name) {
    if (scenes.count(name) == 1) {
        return scenes.at(name);
    }

    const std::filesystem::path scenePath = getScenePath(name);
    if (!std::filesystem::exists(scenePath)) {
        std::cout << "error: scene " << name << " is missing";
        exit(0);
    }

    rapidjson::Document sceneDocument;
    loadFile(scenePath, sceneDocument);
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
        exit(0);
    }

    rapidjson::Document templateDocument;
    loadFile(path, templateDocument);
    templates.emplace(name, std::move(templateDocument));
    return templates.at(name);
}

const rapidjson::Document &DocumentManager::getGameConfig() {
    if (!gameConfig.IsNull()) {
        return gameConfig;
    }

    const std::filesystem::path path = GAME_CONFIG_PATH;
    if (!std::filesystem::exists(path)) {
        std::cout << "error: " << path << " missing";
        exit(0);
    }

    loadFile(path, gameConfig);
    return gameConfig;
}

const rapidjson::Document &DocumentManager::getRenderingConfig() {
    if (!renderingConfig.IsNull()) {
        return renderingConfig;
    }

    const std::filesystem::path path = RENDERING_CONFIG_PATH;
    if (!std::filesystem::exists(path)) {
        renderingConfig.SetObject(); // Returns {} (empty document)
        return renderingConfig;
    }

    loadFile(path, renderingConfig);
    return renderingConfig;
}
