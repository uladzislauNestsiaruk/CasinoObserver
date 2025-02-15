#pragma once

#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/Sprite.hpp"
#include "SFML/System/Vector2.hpp"
#include "SFML/Window/Event.hpp"
#include "../GameStates/state_manager.h"

#include <memory>
#include <unordered_map>

class GameObject {
    using object_ptr = std::unique_ptr<GameObject>;
    using event_handler = void(*)(const sf::RenderWindow& window);

public:
    GameObject() : children_(), active_sprite_(0) {}

    explicit GameObject(const sf::Sprite& sprite) : sprites_(1, sprite), active_sprite_(0) {}

    void AddSprite(const sf::Sprite& sprite) { sprites_.emplace_back(sprite); }

    void AddChild(object_ptr child) { children_.push_back(std::move(child)); }

    void AddHandler(const sf::Event::EventType event_type, event_handler handler) {
        handlers_[event_type] = handler;
    }

    void TriggerHandler(const sf::Event& event);

    void Resize(sf::Vector2u size);

    void Draw(sf::RenderWindow& window);
    
    const sf::Rect<int>& GetSpritesRect() const {
        if (sprites_.empty()) {
            throw std::runtime_error("Attempt Get on empty sprite");
        }

        return sprites_[0].getTextureRect();
    }

private:
    std::vector<object_ptr> children_;
    std::vector<sf::Sprite> sprites_;
    std::unordered_map<sf::Event::EventType, event_handler> handlers_;
    size_t active_sprite_;

    bool Intersect(sf::Vector2f point) noexcept {
        if (active_sprite_ >= sprites_.size()) {
            return false;
        }

        return sprites_[active_sprite_].getGlobalBounds().contains(point);
    }
};
