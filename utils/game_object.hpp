#pragma once

#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "../game_states/state_manager.hpp"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/Sprite.hpp"
#include "SFML/System/Clock.hpp"
#include "SFML/System/Vector2.hpp"
#include "SFML/Window/Event.hpp"
#include "json.hpp"

#define DEFINE_GOHANDLER(NAME)                                                                     \
    void NAME(StateManager& manager, IGameState* state, const nlohmann::json& data);

using nlohmann::json;

class GameObject : public std::enable_shared_from_this<GameObject> {
    using object_ptr = std::shared_ptr<GameObject>;

public:
    using event_handler = void (*)(StateManager* manager, IGameState* state, json& data);

    GameObject() = delete;

    explicit GameObject(const std::string& tag, sf::Vector2f scale, std::string default_phase)
        : tag_(tag), scale_(scale), active_phase_(default_phase) {}

    void AddPhase(std::vector<sf::Sprite>&& sprite, const std::string& phase) {
        phases_[phase] = {std::move(sprite)};
    }

    void AddChild(object_ptr child, const std::optional<std::string>& phase);

    void RemoveChild(const std::string& phase_tag, const std::string& child_tag);

    void AddHandler(const sf::Event::EventType event_type, event_handler handler,
                    const std::string& tag = "");

    std::optional<std::string> TriggerHandler(StateManager* manager, IGameState* state,
                                              nlohmann::json& data);

    void Resize(sf::Vector2u size, sf::Vector2f scale = {-1, -1},
                sf::Vector2f new_parent_size = {1, 1}, sf::Vector2f new_parent_pos = {0, 0},
                sf::Vector2f parent_delta = {1, 1});

    sf::Vector2f GetPosition() const;

    sf::Vector2f GetSize() const;

    const std::string& GetActivePhase() const { return active_phase_; }

    void Move(sf::Vector2f offset);

    void Draw(sf::RenderWindow* window);

    object_ptr FindGameObjectByTag(const std::string& tag);

    bool TryUpdatePhase(const std::string&, uint64_t delay);

private:
    void ResetUnactivePhaseAnimations();

private:
    std::string tag_;
    sf::Vector2f scale_;
    bool resized_ = false;

    std::unordered_map<sf::Event::EventType, event_handler> handlers_;
    std::unordered_map<std::string, std::vector<object_ptr>> children_;
    std::unordered_map<std::string, std::vector<sf::Sprite>> phases_;

    size_t active_sprite_ = 0;
    bool is_finished_current_phase_ = false;
    std::string active_phase_;

    sf::Clock clock_;

    bool Contains(sf::Vector2f point) noexcept;
};
