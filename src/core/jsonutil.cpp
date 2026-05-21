#include "core/jsonutil.h"

#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>

#include <filesystem>
#include <iostream>

rapidjson::Document JsonUtil::loadJsonFile(const std::filesystem::path &path) {
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
    rapidjson::Document outDocument;
    outDocument.ParseStream(stream);
    std::fclose(file_pointer);

    if (outDocument.HasParseError()) {
        rapidjson::ParseErrorCode errorCode = outDocument.GetParseError();
        std::cout << "error parsing json at [" << path << "]" << '\n';
        std::cout << "error code " << errorCode << '\n';
        exit(1);
    }

    return outDocument;
}

void JsonUtil::get(const rapidjson::Value &obj, const char *key, float &out) {
    if (obj.HasMember(key)) {
        out = obj[key].GetFloat();
    }
}

void JsonUtil::get(const rapidjson::Value &obj, const char *key, int &out) {
    if (obj.HasMember(key)) {
        out = obj[key].GetInt();
    }
}

void JsonUtil::get(const rapidjson::Value &obj, const char *key, bool &out) {
    if (obj.HasMember(key)) {
        out = obj[key].GetBool();
    }
}

void JsonUtil::get(const rapidjson::Value &obj, const char *key,
                   std::string &out) {
    if (obj.HasMember(key)) {
        out = obj[key].GetString();
    }
}

void JsonUtil::get(const rapidjson::Value &obj, const char *key, uint8_t &out) {
    if (obj.HasMember(key)) {
        out = static_cast<uint8_t>(obj[key].GetInt());
    }
}

void JsonUtil::get(const rapidjson::Value &obj, const char *key,
                   std::optional<float> &out) {
    if (obj.HasMember(key)) {
        out = obj[key].GetFloat();
    }
}

void JsonUtil::get(const rapidjson::Value &obj, const char *key,
                   std::optional<uint8_t> &out) {
    if (obj.HasMember(key)) {
        out = static_cast<uint8_t>(obj[key].GetInt());
    }
}
