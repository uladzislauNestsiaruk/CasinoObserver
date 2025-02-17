#pragma once

#include <memory>
#include <string>
#include <vector>

#include "SFML/System/Time.hpp"
#include "game_state.h"
#include "state_manager.h"
#include <game_object.hpp>
#include <json.hpp>
#include <thread_safe_queue.hpp>

using json = nlohmann::json;

class WorkRoomState : public IGameState {
    const static std::string kWorkRoomGameObjects;

 public:
    explicit WorkRoomState(StateManager* manager);

    void HandleEvent(const sf::Event& event) override;
    void Update(sf::Time delta) override;
    void Draw(StateManager* manager) override;

    ~WorkRoomState() override {}

 private:
    std::vector<std::shared_ptr<IGameState>> tables_;
    std::shared_ptr<GameObject> root_game_object_;

    mutable size_t total_tables = 12;

    friend void BlackjackScreenHandler(StateManager* manager, IGameState* state, json& data);
    friend void PokerScreenHandler(StateManager* manager, IGameState* state, json& data);
};
