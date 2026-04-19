#pragma once

#include <filesystem>

#ifdef __SWITCH__
const std::filesystem::path RESOURCES_PATH = "romfs:/";
#else
const std::filesystem::path RESOURCES_PATH = "resources/";
#endif

const std::filesystem::path TEMPLATES_PATH = RESOURCES_PATH / "actor_templates";
const std::filesystem::path AUDIO_PATH = RESOURCES_PATH / "audio";
const std::filesystem::path COMPONENTS_PATH =
    RESOURCES_PATH / "component_types";
const std::filesystem::path FONTS_PATH = RESOURCES_PATH / "fonts";
const std::filesystem::path IMAGES_PATH = RESOURCES_PATH / "images";
const std::filesystem::path SCENES_PATH = RESOURCES_PATH / "scenes";
const std::filesystem::path GAME_CONFIG_PATH = RESOURCES_PATH / "game.config";
const std::filesystem::path RENDERING_CONFIG_PATH =
    RESOURCES_PATH / "rendering.config";

inline std::filesystem::path getTemplatePath(const std::string &name) {
    return TEMPLATES_PATH / (name + ".template");
}
inline std::filesystem::path getAudioOggPath(const std::string &name) {
    return AUDIO_PATH / (name + ".ogg");
}
inline std::filesystem::path getAudioWavPath(const std::string &name) {
    return AUDIO_PATH / (name + ".wav");
}
inline std::filesystem::path getComponentPath(const std::string &name) {
    return COMPONENTS_PATH / (name + ".lua");
}
inline std::filesystem::path getFontPath(const std::string &name) {
    return FONTS_PATH / (name + ".ttf");
}
inline std::filesystem::path getImagePath(const std::string &name) {
    return IMAGES_PATH / (name + ".png");
}
inline std::filesystem::path getScenePath(const std::string &name) {
    return SCENES_PATH / (name + ".scene");
}
