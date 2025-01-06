#ifndef CASINOOBSERVER_TEXTURES_LOADER_H
#define CASINOOBSERVER_TEXTURES_LOADER_H

#include "SFML/Graphics/Texture.hpp"

void Preload();

const sf::Texture& GetTextute(const std::string& texture_name);

#endif
