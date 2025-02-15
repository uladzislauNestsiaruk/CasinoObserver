// Copyright [2024] Nestsiarul Uladzislau

#include "poker_close.h"
#include "SFML/Window/Event.hpp"
#include "state_manager.h"

#include <iostream>

PokerClose::PokerClose(StateManager* manager)
    : stop_game_thread_(false), run_game_(false), game_exec_thr_(([this] { GameExecutor(); })),
      table_(std::make_unique<PokerTable>(logs_, render_queue_)) {
}

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
        std::optional<json> log_event = logs_.pop();
        if (log_event == std::nullopt) {
            continue;
        }

        std::cout << "[LOG_EVENT]:\n" << log_event << '\n';
    }
}

void PokerClose::Draw(StateManager* manager) {
    while (!render_queue_.empty()) {
        std::optional<json> render_event = render_queue_.pop();
        if (render_event == std::nullopt) {
            continue;
        }

        // Handle render events
    }
}
