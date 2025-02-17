#pragma once

#include <string>
#include <thread>

#include "../Tables/poker_table.hpp"
#include "game_state.h"
#include <drawer.hpp>
#include <game_object.hpp>
#include <json.hpp>
#include <thread_safe_queue.hpp>

using json = nlohmann::json;

class PokerClose : public IGameState {
    static const std::string kPokerCloseGameObjects;

    enum class Anim;

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
    std::unique_ptr<IDrawer> drawer_;

    TSQueue<json> logs_;
    TSQueue<json> render_queue_;

    std::shared_ptr<GameObject> root_game_object_;
};
