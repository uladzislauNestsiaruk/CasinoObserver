#pragma once

#include <iostream>
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
#include "json.hpp"

#define DEFINE_GOHANDLER(NAME) void NAME(const nlohmann::json& data);

class GameObject {
    using object_ptr = std::shared_ptr<GameObject>;

 public:
    using event_handler = void (*)(const nlohmann::json& data);

    GameObject() = delete;

    explicit GameObject(const std::string& tag, double scale, const std::string& default_phase)
        : tag_(tag), scale_(scale), active_sprite_(0), active_phase_(default_phase) {}

    void AddPhase(std::vector<sf::Sprite>&& sprite, const std::string& phase) {
        phases_[phase] = {std::move(sprite)};
    }

    void AddChild(object_ptr child, const std::optional<std::string>& phase) {
        if (!phase.has_value()) {
            for (auto& [tag, _] : phases_) {
                children_[tag].push_back(child);
            }

            return;
        }

        children_[phase.value()].push_back(child);
    }

    void RemoveChild(const std::string& phase_tag, const std::string& child_tag);

    void AddHandler(const sf::Event::EventType event_type, event_handler handler,
                    const std::string& tag = "");

    std::optional<std::string> TriggerHandler(const nlohmann::json& data);

    void Resize(sf::Vector2u size, sf::Vector2f scale = sf::Vector2f(-1, -1));

    void Move(sf::Vector2f offset);

    void Draw(sf::RenderWindow* window);

    const sf::Rect<int>& GetSpriteRect() const;

 private:
    std::string tag_;
    double scale_;

    std::unordered_map<sf::Event::EventType, event_handler> handlers_;
    std::unordered_map<std::string, std::vector<object_ptr>> children_;
    std::unordered_map<std::string, std::vector<sf::Sprite>> phases_;

    size_t active_sprite_;
    std::string active_phase_;

    bool Contains(sf::Vector2f point) noexcept;
};
