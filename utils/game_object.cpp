#include <algorithm>
#include <memory>
#include <stdexcept>

#include "../GameStates/state_manager.h"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/Sprite.hpp"
#include "SFML/System/Vector2.hpp"
#include "SFML/Window/Event.hpp"
#include "json.hpp"
#include "game_object.hpp"

void GameObject::Resize(sf::Vector2u size, sf::Vector2f scale) {
    for (auto& [tag, sprites] : phases_) {
        for (auto& sprite : sprites) {
            scale =
                scale == sf::Vector2f(-1, -1)
                    ? sf::Vector2f(static_cast<float>(size.x) / sprite.getTexture()->getSize().x,
                                   static_cast<float>(size.y) / sprite.getTexture()->getSize().y)
                    : scale;
            sprite.setScale(scale);
        }
    }

    for (auto& [phase, phased_children] : children_) {
        for (auto& phased_child : phased_children) {
            phased_child->Resize(size, scale);
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

std::optional<std::string> GameObject::TriggerHandler(const nlohmann::json& data) {
    sf::Vector2f point = sf::Vector2f(event.mouseButton.x, event.mouseButton.y);
    for (int32_t ind = children_[active_phase_].size() - 1; ind >= 0; ind--) {
        if (children_[active_phase_][ind]->Contains(point)) {
            return children_[active_phase_][ind]->TriggerHandler(data);
        }
    }

    if (Contains(point) && handlers_.contains(event.type)) {
        handlers_[event.type](data);
        return std::optional<std::string>(tag_);
    }

    return std::nullopt;
}

const sf::Rect<int>& GameObject::GetSpriteRect() const {
    if (active_sprite_ >= phases_.at(active_phase_).size()) {
        throw std::runtime_error("Attempt Get rect of non exisisting sprite in Game object " +
                                 tag_);
    }

    return phases_.at(active_phase_)[active_sprite_].getTextureRect();
}

bool GameObject::Contains(sf::Vector2f point) noexcept {
    if (active_sprite_ >= phases_.at(active_phase_).size()) {
        return false;
    }

    return phases_.at(active_phase_)[active_sprite_].getGlobalBounds().contains(point);
}
