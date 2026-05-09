#pragma once

#include "rapidjson/document.h"
#include "rapidjson/fwd.h"

#include <cstdint>
#include <filesystem>
#include <optional>
#include <string>

namespace JsonUtil {
rapidjson::Document loadJsonFile(const std::filesystem::path &path);

void get(const rapidjson::Value &obj, const char *key, float &out);
void get(const rapidjson::Value &obj, const char *key, int &out);
void get(const rapidjson::Value &obj, const char *key, bool &out);
void get(const rapidjson::Value &obj, const char *key, std::string &out);
void get(const rapidjson::Value &obj, const char *key, uint8_t &out);
void get(const rapidjson::Value &obj, const char *key,
         std::optional<float> &out);
void get(const rapidjson::Value &obj, const char *key,
         std::optional<uint8_t> &out);
} // namespace JsonUtil
