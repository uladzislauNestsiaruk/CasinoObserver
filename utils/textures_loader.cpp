#include <filesystem>
#include <stdexcept>
#include <string>
#include <unordered_map>

#include "textures_loader.h"

std::unordered_map<std::string, sf::Texture> textures;

std::string ExtractName(const std::string& path) {
    size_t last_slash = path.find_last_of('/');
    size_t last_dot = path.find_last_of('.');

    if (last_slash == std::string::npos || last_dot == std::string::npos || last_dot < last_slash) {
        throw std::logic_error("extremely strange texture name in folder images/");
    }

    return path.substr(last_slash + 1, last_dot - last_slash - 1);
}

void Preload() {
    std::filesystem::path images_directory("../../images/");
    for (const auto& entry : std::filesystem::directory_iterator(images_directory)) {
        if (entry.is_regular_file()) {
            textures[ExtractName(entry.path())].loadFromFile(entry.path());
        }
    }
}

const sf::Texture& GetTextute(const std::string& texture_name) {
    if (textures.contains(texture_name)) {
        return textures[texture_name];
    }

    throw std::logic_error("no such texture in preloaded map");
}
