#pragma once

#include <rapidjson/document.h>

#include <string>
#include <unordered_map>

class DocumentManager {
public:
    [[nodiscard]] static const rapidjson::Document &
    getScene(const std::string &name);
    [[nodiscard]] static const rapidjson::Document &
    getTemplate(const std::string &name);

private:
    static inline std::unordered_map<std::string, rapidjson::Document> scenes;
    static inline std::unordered_map<std::string, rapidjson::Document>
        templates;
};
