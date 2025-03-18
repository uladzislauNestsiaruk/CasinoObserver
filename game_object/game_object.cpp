#include <algorithm>
#include <cstddef>
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

void GameObject::ResizeImpl(sf::Vector2f size, sf::Vector2f proportion_vector,
                            bool according_to_parent) {
    sf::Vector2f old_size = GetSize();
    sf::Vector2f position = position_;
    sf::Vector2f old_scale = scale_;

    if (!according_to_parent) {
        scale_ = {size.x / old_size.x, size.y / old_size.y};
    } else {
        scale_ = {scale_.x * proportion_vector.x, scale_.y * proportion_vector.y};
    }

    resized_ = true;

    for (auto& [phase, phased_children] : children_) {
        for (auto& phased_child : phased_children) {
            if (phased_child->resized_) {
                continue;
            }
            sf::Vector2f dist = phased_child->GetPosition() - position;
            sf::Vector2f delta(dist.x / old_size.x, dist.y / old_size.y);
            sf::Vector2f offset = sf::Vector2f(position.x + delta.x * GetSize().x,
                                               position.y + delta.y * GetSize().y) -
                                  phased_child->GetPosition();
            sf::Vector2f proportion_vector(scale_.x / old_scale.x, scale_.y / old_scale.y);
            phased_child->Move(offset);
            phased_child->ResizeImpl(size, proportion_vector, true);
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
    ResizeImpl(size, sf::Vector2f(1, 1), according_to_parent);
    ClearResizeTag();
}

void GameObject::Draw(sf::RenderWindow* window) {
    // Don't draw objects which are in "empty" phase
    if (phases_[active_phase_].empty()) {
        throw std::logic_error("draw called on empty game_object");
    }

    active_sprite_ = std::min(phases_[active_phase_].size() - 1, active_sprite_ + 1);

    phases_[active_phase_][active_sprite_].setTextureRect(visible_rect_);
    phases_[active_phase_][active_sprite_].setPosition(position_);
    phases_[active_phase_][active_sprite_].setScale(scale_);

    window->draw(phases_[active_phase_][active_sprite_]);
    if (active_sprite_ == phases_[active_phase_].size() - 1 && !is_finished_current_phase_) {
        clock_.restart();
        is_finished_current_phase_ = true;
    }

    for (auto child : children_[active_phase_]) {
        child->Draw(window);
    }
}

void GameObject::RemoveChild(const std::string& child_tag,
                             const std::optional<std::string>& phase) {
    auto remove_from_phase = [this, &child_tag](const std::string& phase) {
        for (size_t child_ind = 0; child_ind < children_[phase].size(); child_ind++) {
            if (children_[phase][child_ind]->tag_ == child_tag) {
                children_[phase].erase(children_[phase].begin() + child_ind);
                return;
            }
        }
    };

    if (phase.has_value()) {
        remove_from_phase(phase.value());
    } else {
        for (const auto& phase_pair : phases_) {
            remove_from_phase(phase_pair.first);
        }
    }

    for (auto child_pair : children_) {
        std::for_each(
            child_pair.second.begin(), child_pair.second.end(),
            [phase, child_tag](object_ptr child) { child->RemoveChild(child_tag, phase); });
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
    GameObject* root = this;
    while (root->parent_) {
        root = root->parent_;
    }
    object_ptr existing_copy = root->FindGameObjectByTag(child->GetTag(), std::nullopt);
    if (existing_copy && existing_copy.get() != child.get()) {
        throw std::logic_error(
            "game object with such tag: \"" + child->GetTag() +
            "\" already exists in tree of this game object or one of his parents");
    }

    child->parent_ = this;
    if (phase.has_value()) {
        children_[phase.value()].push_back(child);
        return;
    }

    for (auto& [tag, _] : phases_) {
        children_[tag].push_back(child);
    }
}

void GameObject::Move(sf::Vector2f offset) noexcept {
    position_ += offset;
    for (const auto& phase_pair : phases_) {
        for (const auto& child : children_[phase_pair.first]) {
            child->Move(offset);
        }
    }
}

GameObject::object_ptr
GameObject::FindGameObjectByTag(const std::string& tag,
                                const std::optional<std::string>& phase) noexcept {
    if (tag_ == tag) {
        return shared_from_this();
    }
    auto find_for_phase = [this, &tag](const std::string& phase) {
        object_ptr result = nullptr;
        for (auto child : children_[phase]) {
            result = child->FindGameObjectByTag(tag);
            if (result) {
                break;
            }
        }
        return result;
    };

    if (phase.has_value()) {
        return find_for_phase(phase.value());
    }

    for (const auto& phase_pair : phases_) {
        object_ptr phase_result = find_for_phase(phase_pair.first);
        if (phase_result) {
            return phase_result;
        }
    }

    return nullptr;
}

void GameObject::SetVisibleRect(sf::IntRect visible_rect) {
    if (phases_.empty()) {
        throw std::logic_error(
            "attempt to set visibility rectange on game object without no sprites");
    }
    visible_rect_ = visible_rect;
}

void GameObject::SetScale(sf::Vector2f scale) {
    if (phases_.empty() || !parent_) {
        throw std::logic_error(
            "attempt to set proportion accroding to parent on the spritless or root game object");
    }
    scale_ = scale;
}

sf::Vector2f GameObject::GetSize() const {
    if (phases_.empty()) {
        return sf::Vector2f(0, 0);
    }
    sf::Vector2f active_sprite_size =
        phases_.at(active_phase_)[active_sprite_].getGlobalBounds().getSize();
    return sf::Vector2f(active_sprite_size.x * scale_.x, active_sprite_size.y * scale_.y);
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
