// Copyright [2024] Nestsiarul Uladzislau
#include <iostream>
#include <memory>
#include <string>
#include <thread>

#include "SFML/System/Sleep.hpp"
#include "SFML/System/Time.hpp"
#include "SFML/Window/Event.hpp"
#include "blackjack_close.hpp"
#include "game_state.hpp"
#include "state_manager.hpp"
#include <common_render_handlers.hpp>
#include <game_objects_loader.hpp>
#include <textures_loader.hpp>

const std::string BlackjackClose::kBlackjackCloseGameObjects =
    GetAssetPrefix() + "/game_objects/blackjack_close_objects.json";

BlackjackClose::BlackjackClose(StateManager* manager)
    : AbstractGameState(kBlackjackCloseGameObjects), run_game_(false), stop_game_thread_(false),
      game_executor_(([this] { GameExecutor(); })) {
    table_ = std::make_unique<BlackjackTable>(logs_, render_events_manager_.GetRenderQueue());
    std::shared_ptr<GameObject> root_game_object_ = objects_manager_.FindObjectByTag("root");
    root_game_object_->Resize(manager->GetWindowSize());
    root_game_object_->AddHandler(sf::Event::MouseButtonPressed,
                                  CommonGOEventHandlers::ReturnButtonHandler, "return_button");

    render_events_manager_.AddHandler("change_phase", CommonREMEventHandlers::ChangePhaseHandler);
}

BlackjackClose::~BlackjackClose() {
    stop_game_thread_.store(true);
    game_executor_.join();
}

void BlackjackClose::GameExecutor() {
    while (!stop_game_thread_) {
        if (run_game_.load()) {
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
    objects_manager_.HandleEvent(this, event_data);
}

void BlackjackClose::Update(sf::Time delta) {
    if (table_->IsGameFinished() && logs_.empty() && render_events_manager_.IsEmpty()) {
        run_game_.store(true);
    }

    render_events_manager_.HandleEvent();
    while (!logs_.empty()) {
        std::optional<json> log_event = logs_.pop();
        if (log_event == std::nullopt) {
            continue;
        }
        std::cout << "[LOG EVENT]:\n" << log_event.value() << "\n";
    }
}

void BlackjackClose::Draw(StateManager* manager) {
    sf::sleep(sf::milliseconds(50));
    objects_manager_.DrawAll(manager->GetOriginWindow());
}
