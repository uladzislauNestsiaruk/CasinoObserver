#pragma once

#include <vector>

#include "SFML/Graphics/Sprite.hpp"
#include "SFML/System/Time.hpp"
#include "game_state.h"

class WorkRoomState : public IGameState {
public:
    explicit WorkRoomState(StateManager* manager);

    void HandleEvent(const sf::Event& event) override;
    void Update(sf::Time delta) override;
    void Draw(StateManager* manager) override;

    ~WorkRoomState() override {}

private:
    std::vector<std::shared_ptr<IGameState>> tables_;
    std::vector<sf::Sprite> cameras_;
    mutable size_t total_tables = 12;
};
