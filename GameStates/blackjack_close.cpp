// Copyright [2024] Nestsiarul Uladzislau

#include "blackjack_close.h"
#include "../state_manager.h"
#include "../textures_loader.h"
#include "SFML/Window/Event.hpp"

BlackjackClose::BlackjackClose(StateManager *manager)
    : table_sprite_(GetTextute("blackjack_table")) {
    auto texture_size = GetTextute("blackjack_table").getSize();
    auto window_size = manager->GetWindowSize();

    table_sprite_.setOrigin(texture_size.x / 2.0, texture_size.y / 2.0);
    table_sprite_.setPosition(window_size.x / 2.0, window_size.y / 2.0);
}

void BlackjackClose::HandleEvent(const sf::Event &event) {}

void BlackjackClose::Update(StateManager *manager) {}

void BlackjackClose::Draw(StateManager *manager) {
    manager->DrawSprite(table_sprite_);
}
