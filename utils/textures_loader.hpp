#pragma once

#include "SFML/Graphics/Texture.hpp"

using TexturesRef = std::vector<std::reference_wrapper<sf::Texture>>;

void Preload();

const std::string& GetAssetPrefix();

const sf::Texture& GetTexture(const std::string& texture_name);

TexturesRef GetTextures(const std::string& textures_dir_name);
