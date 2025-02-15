#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>
#include <unordered_map>

#include "textures_loader.h"

std::unordered_map<std::string, sf::Texture> textures;

const std::string kAssetsPaths[] = {"assets/", "../assets/", "../../assets",
                                    "CasinoObserver/assets/"};

std::string ExtractName(const std::string& path) {
    if (!path.ends_with(".png")) {
        throw std::logic_error("extremely strange texture name in folder assets/");
    }

    return path.substr(0, path.size() - strlen(".png"));
}

void PreloadSubdirectory(const std::filesystem::path& path, std::string prefix) {
    for (const auto& entry : std::filesystem::directory_iterator(path)) {
        if (entry.is_regular_file() && std::string(entry.path().filename()).ends_with(".png")) {
            textures[prefix + "_" + ExtractName(entry.path().filename().c_str())].loadFromFile(
                entry.path());
        } else if (entry.is_directory()) {
            PreloadSubdirectory(entry.path(),
                                prefix + "_" + entry.path().parent_path().filename().c_str());
        }
    }
}

void Preload() {
    std::filesystem::path images_directory;

    for (size_t ind = 0; ind < sizeof(kAssetsPaths); ind++) {
        if (std::filesystem::is_directory(kAssetsPaths[ind])) {
            images_directory = kAssetsPaths[ind];
            break;
        }
    }

    if (images_directory.empty()) {
        throw std::runtime_error("unable to find assets folder");
    }

    for (const auto& entry : std::filesystem::directory_iterator(images_directory)) {
        if (entry.is_directory()) {
            PreloadSubdirectory(entry.path(), entry.path().filename().c_str());
        }
    }
}

const sf::Texture& GetTextute(const std::string& texture_name) {
    if (textures.contains(texture_name)) {
        return textures[texture_name];
    }

    throw std::logic_error("no such texture in preloaded map");
}
