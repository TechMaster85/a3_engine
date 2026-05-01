#pragma once

#include "rapidjson/fwd.h"

#include <filesystem>

namespace JsonUtil {
rapidjson::Document loadJsonFile(const std::filesystem::path &path);
}
