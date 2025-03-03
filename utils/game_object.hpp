#pragma once

#include <algorithm>
#include <iostream>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "../game_states/state_manager.hpp"
#include "SFML/Graphics/Rect.hpp"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/Sprite.hpp"
#include "SFML/System/Clock.hpp"
#include "SFML/System/Vector2.hpp"
#include "SFML/Window/Event.hpp"
#include "json.hpp"

#define DEFINE_GOHANDLER(NAME)                                                                     \
    void NAME(StateManager* manager, IGameState* state, GameObject* object, json& data);

using nlohmann::json;

class GameObject : public std::enable_shared_from_this<GameObject> {
    using object_ptr = std::shared_ptr<GameObject>;

public:
    using event_handler = void (*)(StateManager* manager, IGameState* state, GameObject* object,
                                   json& data);

    GameObject() = delete;

    GameObject(const std::string& tag, const std::string& default_phase)
        : tag_(tag), scale_(1, 1), init_scale_(1, 1), active_phase_(default_phase) {}

    GameObject(const std::string& tag, sf::Vector2f init_scale, const std::string& default_phase)
        : tag_(tag), scale_(1, 1), init_scale_(init_scale), active_phase_(default_phase) {}

    GameObject(const std::string& tag, const std::string& default_phase,
               std::vector<sf::Sprite>&& sprite)
        : GameObject(tag, default_phase) {
        if (!sprite.empty()) {
            visible_rect_ = sprite[0].getTextureRect();
        }
        phases_[default_phase] = {std::move(sprite)};
    }

    void AddPhase(std::vector<sf::Sprite>&& sprite, const std::string& phase) {
        if (!sprite.empty()) {
            visible_rect_ = sprite[0].getTextureRect();
        }
        phases_[phase] = {std::move(sprite)};
    }

    void AddChild(object_ptr child, const std::optional<std::string>& phase);

    void RemoveChild(const std::string& phase_tag, const std::string& child_tag);

    void AddHandler(const sf::Event::EventType event_type, event_handler handler,
                    const std::string& tag = "");

    std::optional<std::string> TriggerHandler(StateManager* manager, IGameState* state,
                                              nlohmann::json& data);

    void Resize(sf::Vector2f size, bool according_to_parent = false);

    void SetVisibleRect(sf::IntRect visible_rect) { visible_rect_ = visible_rect; }

    sf::IntRect GetVisibleRect() { return visible_rect_; }

    sf::Vector2f GetPosition() const;

    sf::Vector2f GetSize() const;

    const std::string GetTag() const { return tag_; }

    sf::Vector2f GetScale() const { return scale_; }

    const std::string& GetActivePhase() const { return active_phase_; }

    void Move(sf::Vector2f offset);

    void Draw(sf::RenderWindow* window);

    object_ptr FindGameObjectByTag(const std::string& tag);

    bool TryUpdatePhase(const std::string&, uint64_t delay);

    virtual ~GameObject() {}

private:
    void ResetUnactivePhaseAnimations();

    void MoveSprites(sf::Vector2f);

    void ClearResizeTag();

    void ResizeImpl(sf::Vector2f size, bool according_to_parent);

private:
    std::string tag_;
    bool resized_ = false;
    sf::Vector2f scale_;
    sf::Vector2f init_scale_;
    sf::IntRect visible_rect_;

    std::unordered_map<sf::Event::EventType, event_handler> handlers_;
    std::unordered_map<std::string, std::vector<object_ptr>> children_;
    std::unordered_map<std::string, std::vector<sf::Sprite>> phases_;
    GameObject* parent_ = nullptr;

    size_t active_sprite_ = 0;
    bool is_finished_current_phase_ = false;
    std::string active_phase_;

    sf::Clock clock_;

    bool Contains(sf::Vector2f point) noexcept;
};

bool CorrectEvent(const nlohmann::json& data) noexcept;
