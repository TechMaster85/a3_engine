#pragma once

#include "rapidjson/document.h"

#include <string>
#include <unordered_map>

class DocumentManager {
    static inline std::unordered_map<std::string, rapidjson::Document> scenes;
    static inline std::unordered_map<std::string, rapidjson::Document>
        templates;
    static inline rapidjson::Document gameConfig;      // NOLINT
    static inline rapidjson::Document renderingConfig; // NOLINT

public:
    [[nodiscard]] static const rapidjson::Document &
    getScene(const std::string &name);
    [[nodiscard]] static const rapidjson::Document &
    getTemplate(const std::string &name);
    [[nodiscard]] static const rapidjson::Document &getGameConfig();
    [[nodiscard]] static const rapidjson::Document &getRenderingConfig();
};
