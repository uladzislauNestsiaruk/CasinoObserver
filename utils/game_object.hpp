#pragma once

#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "../GameStates/state_manager.h"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/Sprite.hpp"
#include "SFML/System/Vector2.hpp"
#include "SFML/Window/Event.hpp"

class GameObject {
    using object_ptr = std::unique_ptr<GameObject>;

 public:
    using event_handler = void (*)(StateManager* window, const std::string& child_tag,
                                   IGameState* state);

    GameObject() = delete;

    explicit GameObject(const std::string& tag) : tag_(tag), active_sprite_(0) {}

    GameObject(const std::string& tag, const sf::Sprite& sprite) : GameObject(tag) {
        sprites_.emplace_back(sprite);
    }

    void AddSprite(const sf::Sprite& sprite) { sprites_.emplace_back(sprite); }

    void AddChild(object_ptr child) { children_.push_back(std::move(child)); }

    void AddHandler(const sf::Event::EventType event_type, event_handler handler,
                    const std::string& tag = "");

    std::optional<std::string> TriggerHandler(const sf::Event& event, IGameState* state);

    void Resize(sf::Vector2u size, sf::Vector2f scale = sf::Vector2f(-1, -1));

    void Draw(sf::RenderWindow* window);

    const sf::Rect<int>& GetSpriteRect() const;

 private:
    std::vector<object_ptr> children_;
    std::vector<sf::Sprite> sprites_;
    std::unordered_map<sf::Event::EventType, event_handler> handlers_;
    std::string tag_;
    size_t active_sprite_;

    bool Contains(sf::Vector2f point) noexcept;
};
