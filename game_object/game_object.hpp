#pragma once

#include <animations_manager.hpp>
#include <memory>
#include <optional>
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
    void NAME(StateManager* manager, IGameState* state, GameObject* object, const json& data)

using nlohmann::json;

class GameObject : public std::enable_shared_from_this<GameObject> {
    using object_ptr = std::shared_ptr<GameObject>;

public:
    using event_handler = void (*)(StateManager* manager, IGameState* state, GameObject* object,
                                   const json& data);

    GameObject() = delete;

    GameObject(const std::string& tag, const std::string& default_phase,
               AnimationsManager& animations_manager,
               std::optional<uint64_t> animation_id = std::nullopt)
        : tag_(tag), scale_(1, 1), active_phase_(default_phase),
          animations_manager_(animations_manager),
          animation_id_(animation_id.value_or(animations_manager_.AddNGetAnimationId())) {}

    GameObject(const std::string& tag, sf::Vector2f scale, const std::string& default_phase,
               AnimationsManager& animations_manager,
               std::optional<uint64_t> animation_id = std::nullopt)
        : GameObject(tag, default_phase, animations_manager, animation_id) {
        scale_ = scale;
    }

    void AddPhase(std::vector<sf::Sprite>&& sprite, const std::string& phase) {
        if (!sprite.empty()) {
            visible_rect_ = sprite[0].getTextureRect();
        }
        phases_[phase] = {std::move(sprite)};
    }

    void AddChild(object_ptr child, const std::optional<std::string>& phase);
    void RemoveChild(const std::string& child_tag, const std::optional<std::string>& phase);
    object_ptr FindGameObjectByTag(const std::string& tag,
                                   const std::optional<std::string>& phase = std::nullopt) noexcept;

    void AddHandler(const sf::Event::EventType event_type, event_handler handler,
                    const std::string& tag = "");
    std::optional<std::string> TriggerHandler(StateManager* manager, IGameState* state,
                                              nlohmann::json& data);

    void Resize(sf::Vector2f size, bool according_to_parent = false);

    void SetVisibleRect(sf::IntRect visible_rect);
    sf::IntRect GetVisibleRect() const noexcept { return visible_rect_; }

    sf::Vector2f GetPosition() const noexcept { return position_; };
    sf::Vector2f GetSize() const;
    const std::string& GetTag() const noexcept { return tag_; }
    const std::string& GetActivePhase() const noexcept { return active_phase_; }

    sf::Vector2f GetScale() const noexcept { return scale_; }
    void SetScale(sf::Vector2f scale);

    float GetRotation() const noexcept { return rotation_; }
    void SetRotation(float rotation) noexcept { rotation_ = rotation; }

    void Move(sf::Vector2f offset) noexcept;

    void Draw(sf::RenderWindow* window);

    bool TryUpdatePhase(const std::string& new_phase, uint64_t delay,
                        std::optional<uint64_t> new_animation_id = std::nullopt,
                        std::optional<uint64_t> after_animation_id = std::nullopt);
    void FinishPhase() { is_finished_current_phase_ = true; }

    virtual ~GameObject() {}

private:
    void ResetUnactivePhaseAnimations();

    void ClearResizeTag();

    void ResizeImpl(sf::Vector2f size, sf::Vector2f proportion_vector, bool according_to_parent);

private:
    std::string tag_;

    sf::Vector2f position_;

    sf::Vector2f scale_;
    sf::IntRect visible_rect_;
    bool resized_ = false;

    float rotation_;

    std::unordered_map<sf::Event::EventType, event_handler> handlers_;
    std::unordered_map<std::string, std::vector<object_ptr>> children_;
    std::unordered_map<std::string, std::vector<sf::Sprite>> phases_;
    GameObject* parent_ = nullptr;

    size_t active_sprite_ = 0;
    bool is_finished_current_phase_ = false;
    std::string active_phase_;

    sf::Clock clock_;

    AnimationsManager& animations_manager_;
    uint64_t animation_id_;

    bool Contains(sf::Vector2f point) noexcept;
};

bool CorrectEvent(const nlohmann::json& data) noexcept;
