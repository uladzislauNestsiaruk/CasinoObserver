// Copyright [2024] Nestsiarul Uladzislau

#include "poker_close.h"
#include "state_manager.h"
#include "SFML/Window/Event.hpp"

PokerClose::PokerClose(StateManager* manager) : stop_game_thread_(false), run_game_(false), game_exec_thr_(([this]{ GameExecutor(); })), table_(std::make_unique<PokerTable>()) {}

PokerClose::~PokerClose() {
    stop_game_thread_.store(true);
    game_exec_thr_.join();
}

void PokerClose::GameExecutor() {
    while (!stop_game_thread_) {
        if (run_game_.load()) {
            table_->GameIteration();
            run_game_.store(false);
        }
    }
}

void PokerClose::HandleEvent(const sf::Event& event) {}

void PokerClose::Update(sf::Time delta) {
    if (table_->IsGameFinished() && logs_.empty() && render_queue_.empty()) {
        run_game_.store(true);
    }

    while (!logs_.empty()) {
        std::optional<LogEvent> log_event = logs_.pop();
        if (log_event == std::nullopt) {
            continue;
        }

        // Handle log events
    }
}

void PokerClose::Draw(StateManager* manager) {
    while (!render_queue_.empty()) {
        std::optional<RenderEvent> render_event = render_queue_.pop();
        if (render_event == std::nullopt) {
            continue;
        }

        // Handle render events
    }
}
