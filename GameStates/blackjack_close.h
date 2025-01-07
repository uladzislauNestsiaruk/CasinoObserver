// Copyright [2024] Nestsiaruk Uladzislau
#ifndef CASINOONSERVER_GAMESTATES_BLACKJACK_CLOSE_H
#define CASINOONSERVER_GAMESTATES_BLACKJACK_CLOSE_H

#include "../Tables/blackjack_table.h"
#include "../game_state.h"
#include "SFML/Graphics/Sprite.hpp"
#include "SFML/Window/Event.hpp"
#include "animation.h"

class BlackjackClose : public IGameState {
    enum class State { DEALING, PLAYING};

public:
    explicit BlackjackClose(StateManager* manager);

    void HandleEvent(const sf::Event& event) override;
    void Update(sf::Time delta) override;
    void Draw(StateManager* manager) override;

    ~BlackjackClose() override {}

private:
    BlackjackTable table_;
    Animation dealing_animation_;
    State game_state_;
};

#endif // CASINOONSERVER_GAMESTATES_BLACKJACK_CLOSE_H
