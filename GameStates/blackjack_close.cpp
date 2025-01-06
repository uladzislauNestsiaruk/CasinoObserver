// Copyright [2024] Nestsiarul Uladzislau

#include "blackjack_close.h"
#include "../state_manager.h"
#include "SFML/Window/Event.hpp"
#include "textures_loader.h"

BlackjackClose::BlackjackClose(StateManager* manager)
    : dealer_sprite_(GetTextute("male_dealer1")), table_sprite_(GetTextute("blackjack_table")),
      table_() {
    auto table_size = GetTextute("blackjack_table").getSize();
    auto dealer_size = GetTextute("male_dealer1").getSize();
    auto window_size = manager->GetWindowSize();

    table_sprite_.setOrigin(table_size.x / 2.0, table_size.y / 2.0);
    table_sprite_.setPosition(window_size.x / 2.0, window_size.y / 2.0);

    dealer_sprite_.setOrigin(dealer_size.x / 2.0, dealer_size.y / 2.0);
    dealer_sprite_.setPosition(window_size.x / 2.0 - dealer_size.x / 2.0,
                               table_sprite_.getPosition().y - dealer_size.y / 2.5);
}

void BlackjackClose::HandleEvent(const sf::Event& event) {}

void BlackjackClose::Update(StateManager* manager) {}

void BlackjackClose::Draw(StateManager* manager) {
    manager->DrawSprite(dealer_sprite_);
    manager->DrawSprite(table_sprite_);
}
