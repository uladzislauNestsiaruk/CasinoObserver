#pragma once

#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/Sprite.hpp"
#include "SFML/System/Vector2.hpp"
#include "SFML/Window/Event.hpp"

#include <memory>
#include <unordered_map>

class GameObject {
    using object_ptr = std::shared_ptr<GameObject>;
    using event_handler = void(*)(const sf::RenderWindow& window);

public:
    GameObject() : children_(), active_sprite_(0) {
    }

    explicit GameObject(const sf::Sprite& sprite) : sprites_(1, sprite), active_sprite_(0) {
    }

    void AddSprite(const sf::Sprite& sprite) {
        sprites_.emplace_back(sprite);
    }

    void AddChild(object_ptr child) {
        children_.emplace_back(child);
    }

    void AddHandler(const sf::Event& event, event_handler handler) {
        handlers_[event] = handler;
    }

    void Resize(sf::Vector2u size);

    void Draw(sf::RenderWindow& window);
    
private:
    std::vector<object_ptr> children_;
    std::vector<sf::Sprite> sprites_;
    std::unordered_map<sf::Event, event_handler> handlers_;
    size_t active_sprite_;
};
