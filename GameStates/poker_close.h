// Copyright [2024] Nestsiaruk Uladzislau
#ifndef CASINOONSERVER_GAMESTATES_POKER_CLOSE_H
#define CASINOONSERVER_GAMESTATES_POKER_CLOSE_H

#include "../game_state.h"

class PokerClose : public IGameState {
public:
    explicit PokerClose(StateManager* manager);

    void HandleEvent(const sf::Event& event) override;
    void Update(StateManager* manager) override;
    void Draw(StateManager* manager) override;

    ~PokerClose() override {}

private:
};

#endif // CASINOONSERVER_GAMESTATES_POKER_CLOSE_H
