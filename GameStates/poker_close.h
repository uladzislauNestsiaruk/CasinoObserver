#pragma once

#include "game_state.h"

#include "../Tables/poker_table.h"

#include "drawer.hpp"
#include "thread_safe_queue.hpp"

#include <thread>

class PokerClose : public IGameState {
public:
    explicit PokerClose(StateManager* manager);

    void HandleEvent(const sf::Event& event) override;
    void Update(sf::Time delta) override;
    void Draw(StateManager* manager) override;

    ~PokerClose() override;

private:
    void GameExecutor();

private:
    struct LogEvent {};
    struct RenderEvent {};

    std::atomic<bool> stop_game_thread_;
    std::atomic<bool> run_game_;
    std::thread game_exec_thr_;

    std::unique_ptr<ITable> table_;
    std::unique_ptr<IDrawer> drawer_;

    TSQueue<LogEvent> logs_;
    TSQueue<RenderEvent> render_queue_;
};

