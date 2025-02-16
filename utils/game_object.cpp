#include <memory>
#include <stdexcept>

#include "../GameStates/state_manager.h"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/System/Vector2.hpp"
#include "SFML/Window/Event.hpp"
#include "game_object.hpp"

void GameObject::Resize(sf::Vector2u size, sf::Vector2f scale) {
    for (auto& sprite : sprites_) {
        scale = scale == sf::Vector2f(-1, -1)
                    ? sf::Vector2f(static_cast<float>(size.x) / sprite.getTexture()->getSize().x,
                                   static_cast<float>(size.y) / sprite.getTexture()->getSize().y)
                    : scale;
        sprite.setScale(scale);
    }

    for (auto& child : children_) {
        child->Resize(size, scale);
    }
}

void GameObject::Draw(sf::RenderWindow* window) {
    if (sprites_.empty()) {
        throw std::logic_error("draw called on empty game_object");
    }

    window->draw(sprites_[active_sprite_++]);
    active_sprite_ %= sprites_.size();

    for (size_t i = 0; i < children_.size(); ++i) {
        children_[i]->Draw(window);
    }
}

void GameObject::AddHandler(const sf::Event::EventType type, event_handler handler,
                            const std::string& tag) {
    if (tag == tag_) {
        handlers_[type] = handler;
        return;
    }

    for (size_t ind = 0; ind < children_.size(); ind++) {
        children_[ind]->AddHandler(type, handler, tag);
    }
}

std::optional<std::string> GameObject::TriggerHandler(const sf::Event& event, IGameState* state) {
    sf::Vector2f point = sf::Vector2f(event.mouseButton.x, event.mouseButton.y);
    for (int32_t ind = children_.size() - 1; ind >= 0; ind--) {
        if (children_[ind]->Contains(point)) {
            return children_[ind]->TriggerHandler(event, state);
        }
    }

    if (Contains(point) && handlers_.contains(event.type)) {
        handlers_[event.type](&StateManager::Instance(), tag_, state);
        return std::optional<std::string>(tag_);
    }

    return std::nullopt;
}

const sf::Rect<int>& GameObject::GetSpriteRect() const {
    if (active_sprite_ >= sprites_.size()) {
        throw std::runtime_error("Attempt Get rect of non exisisting sprite in Game object " +
                                 tag_);
    }

    return sprites_[active_sprite_].getTextureRect();
}

bool GameObject::Contains(sf::Vector2f point) noexcept {
    if (active_sprite_ >= sprites_.size()) {
        return false;
    }

    return sprites_[active_sprite_].getGlobalBounds().contains(point);
}
