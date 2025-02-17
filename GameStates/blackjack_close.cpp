// Copyright [2024] Nestsiarul Uladzislau
#include <memory>
#include <string>

#include "SFML/Window/Event.hpp"
#include "blackjack_close.hpp"
#include "state_manager.h"
#include <game_objects_loader.hpp>
#include <textures_loader.hpp>

const std::string BlackjackClose::kBlackjackCloseGameObjects =
    GetAssetPrefix() + "/game_objects/blackjack_close_objects.json";

static void ReturnButtonHandler(StateManager* manager, IGameState* state, nlohmann::json& data) {
    manager->Pop();
}

BlackjackClose::BlackjackClose(StateManager* manager)
    : table_(std::make_unique<BlackjackTable>(logs_, render_queue_)), drawer_(), run_game_(false),
      stop_game_thread_(false), game_executor_(([this] { GameExecutor(); })), logs_(),
      render_queue_(), root_game_object_(ParseGameObjects(kBlackjackCloseGameObjects)) {
    root_game_object_->Resize(manager->GetWindowSize());
    root_game_object_->AddHandler(sf::Event::MouseButtonPressed, ReturnButtonHandler,
                                  "return_button");
}

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

void BlackjackClose::HandleEvent(const sf::Event& event) {
    if (event.type != sf::Event::MouseButtonPressed) {
        return;
    }

    json event_data;
    event_data["event"]["mouse_button"]["x"] = event.mouseButton.x;
    event_data["event"]["mouse_button"]["y"] = event.mouseButton.y;
    event_data["event"]["type"] = event.type;
    root_game_object_->TriggerHandler(&StateManager::Instance(), this, event_data);
}

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

void BlackjackClose::Draw(StateManager* manager) {
    root_game_object_->Draw(manager->GetOriginWindow());
}
