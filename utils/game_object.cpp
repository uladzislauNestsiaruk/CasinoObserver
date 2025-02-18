#include <algorithm>
#include <cstddef>
#include <iostream>
#include <memory>
#include <optional>
#include <stdexcept>

#include "../GameStates/state_manager.h"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/Sprite.hpp"
#include "SFML/System/Time.hpp"
#include "SFML/System/Vector2.hpp"
#include "SFML/Window/Event.hpp"
#include "game_object.hpp"
#include "json.hpp"

void GameObject::Resize(sf::Vector2u size, sf::Vector2f scale, sf::Vector2f new_parent_size,
                        sf::Vector2f new_parent_pos, sf::Vector2f parent_delta) {
    sf::Vector2f old_pos = GetPosition();
    sf::Vector2f old_size = GetSize();
    bool is_root = scale == sf::Vector2f(-1, -1);
    for (auto& [tag, sprites] : phases_) {
        for (auto& sprite : sprites) {
            sf::Vector2f sprite_pos = static_cast<sf::Vector2f>(sprite.getPosition());
            if (!is_root) {
                sprite.setPosition({new_parent_pos.x + parent_delta.x * new_parent_size.x,
                                    new_parent_pos.y + parent_delta.y * new_parent_size.y});
            } else {
                scale = {static_cast<float>(size.x) / sprite.getTexture()->getSize().x,
                         static_cast<float>(size.y) / sprite.getTexture()->getSize().y};
            }
            sprite.setScale(scale);
        }
    }

    sf::Vector2f new_size = GetSize();
    sf::Vector2f new_pos = GetPosition();
    for (auto& [phase, phased_children] : children_) {
        for (auto& phased_child : phased_children) {
            sf::Vector2f delta = phased_child->GetPosition() - old_pos;
            delta = {delta.x / old_size.x, delta.y / old_size.y};
            phased_child->Resize(size, scale, new_size, new_pos, delta);
        }
    }
}

void GameObject::Move(sf::Vector2f offset) {
    for (auto& phase_pair : phases_) {
        std::for_each(phase_pair.second.begin(), phase_pair.second.end(),
                      [offset](sf::Sprite& sprite) { sprite.move(offset); });
    }
    for (auto& child_pair : children_) {
        std::for_each(child_pair.second.begin(), child_pair.second.end(),
                      [offset](object_ptr child) { child->Move(offset); });
    }
}

void GameObject::Draw(sf::RenderWindow* window) {
    if (phases_[active_phase_].empty()) {
        throw std::logic_error("draw called on empty game_object");
    }

    active_sprite_ = std::min(phases_[active_phase_].size() - 1, active_sprite_ + 1);
    window->draw(phases_[active_phase_][active_sprite_]);
    if (active_sprite_ == phases_[active_phase_].size() - 1) {
        is_finished_current_phase_ = true;
    }

    for (auto child : children_[active_phase_]) {
        child->Draw(window);
    }
}

void GameObject::RemoveChild(const std::string& phase_tag, const std::string& child_tag) {
    for (size_t child_ind = 0; child_ind < children_.size(); child_ind++) {
        if (children_[phase_tag][child_ind]->tag_ == child_tag) {
            children_[phase_tag][child_ind]->~GameObject();
            children_[phase_tag].erase(children_[phase_tag].begin() + child_ind);
            return;
        }
    }

    for (auto& child_pair : children_) {
        std::for_each(
            child_pair.second.begin(), child_pair.second.end(),
            [phase_tag, child_tag](object_ptr child) { child->RemoveChild(phase_tag, child_tag); });
    }
}

void GameObject::AddHandler(const sf::Event::EventType type, event_handler handler,
                            const std::string& tag) {
    if (tag == tag_) {
        handlers_[type] = handler;
        return;
    }

    for (auto& [phase_, phased_children] : children_)
        for (auto phased_child : phased_children) {
            phased_child->AddHandler(type, handler, tag);
        }
}

std::optional<std::string> GameObject::TriggerHandler(StateManager* manager, IGameState* state,
                                                      nlohmann::json& data) {
    sf::Vector2f point =
        sf::Vector2f(data["event"]["mouse_button"]["x"], data["event"]["mouse_button"]["y"]);
    for (int32_t ind = children_[active_phase_].size() - 1; ind >= 0; ind--) {
        if (children_[active_phase_][ind]->Contains(point)) {
            return children_[active_phase_][ind]->TriggerHandler(manager, state, data);
        }
    }

    if (Contains(point) && handlers_.contains(data["event"]["type"])) {
        data["child_tag"] = tag_;
        handlers_[data["event"]["type"]](manager, state, data);
        return std::optional<std::string>(tag_);
    }

    return std::nullopt;
}

bool GameObject::Contains(sf::Vector2f point) noexcept {
    if (active_sprite_ >= phases_.at(active_phase_).size()) {
        return false;
    }

    return phases_.at(active_phase_)[active_sprite_].getGlobalBounds().contains(point);
}

GameObject::object_ptr GameObject::FindGameObjectByTag(const std::string& tag) {
    if (tag_ == tag) {
        return shared_from_this();
    }

    for (const auto& [phase, phased_children] : children_) {
        for (auto phased_child : phased_children) {
            object_ptr child_result = phased_child->FindGameObjectByTag(tag);
            if (child_result != nullptr) {
                return child_result;
            }
        }
    }

    return nullptr;
}

sf::Vector2f GameObject::GetPosition() const {
    return phases_.at(active_phase_)[active_sprite_].getPosition();
}

sf::Vector2f GameObject::GetSize() const {
    return phases_.at(active_phase_)[active_sprite_].getGlobalBounds().getSize();
}

bool GameObject::TryUpdatePhase(const std::string& new_phase, uint64_t delay) {
    if (is_finished_current_phase_ && clock_.getElapsedTime() >= sf::milliseconds(delay)) {
        active_phase_ = new_phase;
        active_sprite_ = 0;
        is_finished_current_phase_ = false;
        clock_.restart();
        return true;
    }

    return false;
}
