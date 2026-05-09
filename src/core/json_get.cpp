#include "core/jsonutil.h"

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
