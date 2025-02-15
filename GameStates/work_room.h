#pragma once

#include <vector>

#include "SFML/Graphics/Sprite.hpp"
#include "SFML/System/Time.hpp"
#include "game_state.h"
#include <game_object.hpp>

class WorkRoomState : public IGameState {
    static constexpr std::string_view kWorkRoomGameObjects = "../assets/game_objects/work_room_objects.json";
public:
    explicit WorkRoomState(StateManager* manager);

    void HandleEvent(const sf::Event& event) override;
    void Update(sf::Time delta) override;
    void Draw(StateManager* manager) override;

    ~WorkRoomState() override {}

private:
    std::vector<std::shared_ptr<IGameState>> tables_;
    std::unique_ptr<GameObject> root_game_object_;
    mutable size_t total_tables = 12;
};
