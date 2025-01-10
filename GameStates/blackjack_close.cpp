// Copyright [2024] Nestsiarul Uladzislau

#include "blackjack_close.h"
#include "SFML/Window/Event.hpp"
#include "state_manager.h"
#include "textures_loader.h"

BlackjackClose::BlackjackClose(StateManager* manager)
    : table_(), dealing_animation_(GetTextute("blackjack_dealing_animation"), 4, 3),
      game_state_(State::DEALING) {
    
}

void BlackjackClose::HandleEvent(const sf::Event& event) {}

void BlackjackClose::Update(sf::Time delta) {
    if (game_state_ == State::DEALING) {
        dealing_animation_.Update(delta);
        if (dealing_animation_.IsFinished()) {
            game_state_ = State::PLAYING;
            table_.Dealing();
        }
        return;
    }

    if (!table_.IsGameFinished()) {
        table_.Update(delta);
    }
}

void BlackjackClose::Draw(StateManager* manager) { dealing_animation_.Draw(manager); }
