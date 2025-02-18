#include "SFML/Graphics/Texture.hpp"
#include <filesystem>
#include <stdexcept>
#include <string>
#include <unordered_map>

#include <constants.hpp>
#include <textures_loader.hpp>

const char* kAssetsPrefix[] = {"assets/", "../assets/", "../../assets/", "CasinoObserver/assets/"};

static std::unordered_map<std::string, sf::Texture> textures;

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
            PreloadSubdirectory(entry.path(), prefix + "_" + entry.path().filename().c_str());
        }
    }
}

void Preload() {
    std::filesystem::path images_directory;

    for (size_t ind = 0; ind < sizeof(kAssetsPrefix); ind++) {
        if (std::filesystem::is_directory(kAssetsPrefix[ind])) {
            images_directory = kAssetsPrefix[ind];
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

const std::string& GetAssetPrefix() {
    static std::string prefix;
    if (!prefix.empty()) {
        return prefix;
    }

    for (const std::string& asset_prefix : kAssetsPrefix) {
        if (std::filesystem::exists(asset_prefix)) {
            prefix = asset_prefix;
        }
    }

    return prefix;
}

const sf::Texture& GetTextute(const std::string& texture_name) {
    if (textures.contains(texture_name)) {
        return textures[texture_name];
    }

    throw std::logic_error("no such texture as \"" + texture_name + "\" in preloaded map");
}

TexturesRef GetTextures(const std::string& textures_dir_name) {
    TexturesRef result;
    for (size_t i = 1;; ++i) {
        std::string texture_name = textures_dir_name + "_" + std::to_string(i);
        if (!textures.contains(texture_name)) {
            break;
        }

        result.push_back(textures[texture_name]);
    }

    return result;
}
