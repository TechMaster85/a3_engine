#pragma once

#include "rapidjson/fwd.h"

#include <filesystem>

rapidjson::Document loadJsonFile(const std::filesystem::path& path);
