#include "core/jsonutil.h"

#include <filesystem>
#include <iostream>
#include <rapidjson/filereadstream.h>
#include <rapidjson/document.h>

rapidjson::Document JsonUtil::loadJsonFile(const std::filesystem::path& path) {
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
