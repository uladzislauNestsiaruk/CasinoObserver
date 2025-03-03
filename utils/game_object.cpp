#include <algorithm>
#include <cstddef>
#include <iostream>
#include <memory>
#include <optional>
#include <queue>
#include <stdexcept>

#include "../game_states/state_manager.hpp"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/Sprite.hpp"
#include "SFML/System/Time.hpp"
#include "SFML/System/Vector2.hpp"
#include "SFML/Window/Event.hpp"
#include "game_object.hpp"
#include "json.hpp"

void GameObject::ResizeImpl(sf::Vector2f size, bool according_to_parent) {
    sf::Vector2f old_size = GetSize();
    sf::Vector2f position = GetPosition();
    sf::Vector2f old_scale = scale_;
    for (auto& [tag, sprites] : phases_) {
        for (auto& sprite : sprites) {
            if (!according_to_parent) {
                scale_ = {static_cast<float>(size.x) / sprite.getTexture()->getSize().x,
                          static_cast<float>(size.y) / sprite.getTexture()->getSize().y};
            } else {
                scale_ = {parent_->scale_.x * init_scale_.x, parent_->scale_.y * init_scale_.y};
            }
            sprite.setScale(scale_);
        }
    }
    resized_ = true;

    for (auto& [phase, phased_children] : children_) {
        for (auto& phased_child : phased_children) {
            if (phased_child->resized_) {
                continue;
            }
            sf::Vector2f dist = phased_child->GetPosition() - position;
            sf::Vector2f delta = {dist.x / old_size.x, dist.y / old_size.y};
            sf::Vector2f offset = sf::Vector2f(position.x + delta.x * GetSize().x,
                                               position.y + delta.y * GetSize().y) -
                                  phased_child->GetPosition();
            phased_child->MoveSprites(offset);
            phased_child->ResizeImpl(size, true);
        }
    }
}

void GameObject::ClearResizeTag() {
    resized_ = false;
    for (auto& [phase, phased_children] : children_) {
        for (auto phased_child : phased_children) {
            if (phased_child->resized_) {
                phased_child->ClearResizeTag();
            }
        }
    }
}

void GameObject::Resize(sf::Vector2f size, bool according_to_parent) {
    ResizeImpl(size, according_to_parent);
    ClearResizeTag();
}

void GameObject::MoveSprites(sf::Vector2f offset) {
    for (auto& phase_pair : phases_) {
        std::for_each(phase_pair.second.begin(), phase_pair.second.end(),
                      [offset, this](sf::Sprite& sprite) { sprite.move(offset); });
    }
}

void GameObject::Move(sf::Vector2f offset) {
    MoveSprites(offset);
    for (auto child_pair : children_) {
        std::for_each(child_pair.second.begin(), child_pair.second.end(),
                      [offset](object_ptr child) { child->Move(offset); });
    }
}

void GameObject::Draw(sf::RenderWindow* window) {
    // Don't draw objects which are in "empty" phase
    if (phases_[active_phase_].empty()) {
        throw std::logic_error("draw called on empty game_object");
    }

    active_sprite_ = std::min(phases_[active_phase_].size() - 1, active_sprite_ + 1);
    phases_[active_phase_][active_sprite_].setTextureRect(visible_rect_);
    window->draw(phases_[active_phase_][active_sprite_]);
    if (active_sprite_ == phases_[active_phase_].size() - 1 && !is_finished_current_phase_) {
        clock_.restart();
        is_finished_current_phase_ = true;
    }

    for (auto child : children_[active_phase_]) {
        child->Draw(window);
    }
}

void GameObject::RemoveChild(const std::string& phase_tag, const std::string& child_tag) {
    for (size_t child_ind = 0; child_ind < children_[phase_tag].size(); child_ind++) {
        if (children_[phase_tag][child_ind]->tag_ == child_tag) {
            children_[phase_tag].erase(children_[phase_tag].begin() + child_ind);
            return;
        }
    }

    for (auto child_pair : children_) {
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
    // Don't trigger game object handler if it's in empty phase
    if (active_phase_ == "empty") {
        return std::nullopt;
    }

    sf::Vector2f point = sf::Vector2f(data["event"]["x"], data["event"]["y"]);
    std::optional<std::string> handler_tag;
    for (int32_t ind = children_[active_phase_].size() - 1; ind >= 0 && !handler_tag.has_value();
         ind--) {
        if (children_[active_phase_][ind]->active_phase_ != "empty" &&
            children_[active_phase_][ind]->Contains(point)) {
            handler_tag = children_[active_phase_][ind]->TriggerHandler(manager, state, data);
        }
    }

    if (Contains(point) && handlers_.contains(data["event"]["type"]) && !handler_tag.has_value()) {
        data["tag"] = tag_;
        handlers_[data["event"]["type"]](manager, state, this, data);
        handler_tag = tag_;
    }

    return handler_tag;
}

bool GameObject::Contains(sf::Vector2f point) noexcept {
    if (active_sprite_ >= phases_.at(active_phase_).size()) {
        return false;
    }

    return phases_.at(active_phase_)[active_sprite_].getGlobalBounds().contains(point);
}

void GameObject::AddChild(object_ptr child, const std::optional<std::string>& phase) {
    if (!phase.has_value()) {
        for (auto& [tag, _] : phases_) {
            child->parent_ = this;
            children_[tag].push_back(child);
        }

        return;
    }

    child->parent_ = this;
    children_[phase.value()].push_back(child);
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
    if (active_phase_ == "empty" ||
        (is_finished_current_phase_ && clock_.getElapsedTime() >= sf::milliseconds(delay))) {
        active_phase_ = new_phase;
        active_sprite_ = 0;
        is_finished_current_phase_ = false;
        clock_.restart();
        ResetUnactivePhaseAnimations();
        return true;
    }

    return false;
}

void GameObject::ResetUnactivePhaseAnimations() {
    std::unordered_map<std::string, bool> is_good_object;
    std::queue<object_ptr> object_queue;
    for (const auto& [phase, sprites] : phases_) {
        for (auto child : children_[phase]) {
            is_good_object[child->tag_] = phase == active_phase_;
            object_queue.push(child);
        }
    }

    while (!object_queue.empty()) {
        object_ptr item = object_queue.front();
        object_queue.pop();
        if (is_good_object.contains(item->tag_)) {
            continue;
        }

        const bool current_state = is_good_object[item->tag_];
        if (!current_state) {
            active_sprite_ = 0;
            is_finished_current_phase_ = false;
            clock_.restart();
        }

        for (const auto& [phase, sprites] : item->phases_) {
            for (auto child : children_[phase]) {
                if (!is_good_object.contains(child->tag_)) {
                    object_queue.push(child);
                }
                is_good_object[child->tag_] |= current_state;
            }
        }
    }
}

bool CorrectEvent(const nlohmann::json& data) noexcept {
    if (!data.contains("event") || !data["event"].contains("type")) {
        return false;
    }

    sf::Event::EventType event_type =
        static_cast<sf::Event::EventType>(data["event"]["type"].template get<int32_t>());
    if (event_type == sf::Event::MouseMoved || event_type == sf::Event::MouseButtonPressed ||
        event_type == sf::Event::MouseButtonReleased) {
        return data["event"].contains("x") && data["event"].contains("y");
    }

    if (event_type == sf::Event::MouseWheelScrolled) {
        return data["event"].contains("delta");
    }

    return false;
}
