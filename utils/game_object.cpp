#include "game_object.hpp"
#include "../GameStates/state_manager.h"
#include "SFML/Graphics.hpp"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/System/Vector2.hpp"
#include "SFML/Window/Event.hpp"
#include <optional>
#include <stdexcept>

void GameObject::Resize(sf::Vector2u size) {
    for (auto& sprite : sprites_) {
        sf::Vector2f scale =
            sf::Vector2f(static_cast<float>(size.x) / sprite.getTexture()->getSize().x,
                         static_cast<float>(size.y) / sprite.getTexture()->getSize().y);
        sprite.setScale(scale);
    }

    for (size_t i = 0; i < children_.size(); ++i) {
        children_[i]->Resize(size);
    }
}

void GameObject::Draw(sf::RenderWindow& window) {
    if (sprites_.empty()) {
        throw std::logic_error("draw called on empty game_object");
    }

    window.draw(sprites_[active_sprite_++]);
    active_sprite_ %= sprites_.size();

    for (size_t i = 0; i < children_.size(); ++i) {
        children_[i]->Draw(window);
    }
}

void GameObject::TriggerHandler(const sf::Event& event) {
    sf::Vector2f point = sf::Vector2f(event.mouseButton.x, event.mouseButton.y);
    for (int32_t ind = children_.size() - 1; ind >= 0; ind--) {
        if (children_[ind]->Intersect(point)) {
            children_[ind]->TriggerHandler(event);
            return;
        }
    }

    if (Intersect(point) && handlers_.contains(event.type)) {
        handlers_[event.type](StateManager::Instance().GetOriginWindow());
    }
}
