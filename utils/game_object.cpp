#include "game_object.hpp"
#include "SFML/Graphics.hpp"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/System/Vector2.hpp"
#include <stdexcept>

void GameObject::Resize(sf::Vector2u size) {
    for (auto& sprite : sprites_) {
        sf::Vector2f scale =
            sf::Vector2f(static_cast<float>(size.x) / sprite.getTexture()->getSize().x,
                         static_cast<float>(size.y) / sprite.getTexture()->getSize().y);
        sprite.setScale(scale);
    }

    for (auto child : children_) {
        child->Resize(size);
    }
}

void GameObject::Draw(sf::RenderWindow& window) {
    if (sprites_.empty()) {
        throw std::logic_error("draw called on empty game_object");
    }

    window.draw(sprites_[active_sprite_++]);
    active_sprite_ %= sprites_.size();

    for (auto child : children_) {
        child->Draw(window);
    }
}
