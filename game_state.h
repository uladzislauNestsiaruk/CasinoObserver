// "Copyright [2024] Netsiaruk Uladzislau"
#ifndef CASINOOBSERVER_GAME_SATE_H
#define CASINOOBSERVER_GAME_SATE_H

#include "SFML/Window/Event.hpp"
#include <memory>

class StateManager;

class GameState {
  public:
    GameState() {}

    virtual void HandleEvent(const sf::Event &event) = 0;
    virtual void Update(StateManager *manager) = 0;
    virtual void Draw(StateManager *manager) = 0;

    void ChangeState();

    virtual ~GameState() {}
};

#endif // CASINOOBSERVER_GAME_SATE_H
