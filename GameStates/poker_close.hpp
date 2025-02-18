#pragma once

#include <string>
#include <thread>

#include "../Tables/poker_table.hpp"
#include "game_state.hpp"
#include <game_object.hpp>
#include <json.hpp>
#include <render_events_manager.hpp>
#include <thread_safe_queue.hpp>

using json = nlohmann::json;

class PokerClose : public AbstractGameState {
    static const std::string kPokerCloseGameObjects;
public:
    explicit PokerClose(StateManager* manager);

    void HandleEvent(const sf::Event& event) override;
    void Update(sf::Time delta) override;
    void Draw(StateManager* manager) override;

    ~PokerClose() override;

private:
    void GameExecutor();

private:
    std::atomic<bool> stop_game_thread_;
    std::atomic<bool> run_game_;
    std::thread game_exec_thr_;

    std::unique_ptr<ITable> table_;

    TSQueue<json> logs_;
};
