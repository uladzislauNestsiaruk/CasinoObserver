// Copyright [2024] Nestsiarul Uladzislau

#include <memory>

#include "blackjack_close.h"
#include "SFML/Window/Event.hpp"
#include "state_manager.h"
#include "textures_loader.h"

BlackjackClose::BlackjackClose(StateManager* manager)
    : table_(std::make_unique<BlackjackTable>(logs_, render_queue_)), drawer_(), run_game_(false),
      stop_game_thread_(false), game_executor_(([this] { GameExecutor(); })), logs_(),
      render_queue_(), dealing_animation_(GetTextute("blackjack_dealing_animation"), 4, 3) {}

BlackjackClose::~BlackjackClose() {
    stop_game_thread_.store(false);
    game_executor_.join();
}

void BlackjackClose::GameExecutor() {
    if (!stop_game_thread_) {
        if (run_game_) {
            table_->GameIteration();
            run_game_.store(false);
        }
    }
}

void BlackjackClose::HandleEvent(const sf::Event& event) {}

void BlackjackClose::Update(sf::Time delta) {
    if (table_->IsGameFinished() && logs_.empty() && render_queue_.empty()) {
        run_game_.store(true);
    }

    while (!logs_.empty()) {
        std::optional<json> log_event = logs_.pop();
        if (log_event == std::nullopt) {
            continue;
        }
    }
}

void BlackjackClose::Draw(StateManager* manager) { dealing_animation_.Draw(manager); }
