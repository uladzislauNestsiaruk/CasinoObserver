// Copyright [2024] Nestsiaruk Uladzislau
#ifndef CASINOONSERVER_GAMESTATES_BLACKJACK_CLOSE_H
#define CASINOONSERVER_GAMESTATES_BLACKJACK_CLOSE_H

#include "../game_state.h"
#include "SFML/Graphics/Sprite.hpp"
#include "SFML/Window/Event.hpp"

class BlackjackClose : public IGameState {
public:
    explicit BlackjackClose(StateManager *manager);

    void HandleEvent(const sf::Event &event) override;
    void Update(StateManager *manager) override;
    void Draw(StateManager *manager) override;

    ~BlackjackClose() override {}

private:
    sf::Sprite dealer_sprite_;
    sf::Sprite table_sprite_;
};

#endif // CASINOONSERVER_GAMESTATES_BLACKJACK_CLOSE_H
