// Copyright [2024] Nestsiarul Uladzislau

#include "poker_close.hpp"
#include "SFML/System/Time.hpp"
#include "SFML/Window/Event.hpp"
#include "game_state.h"
#include "state_manager.h"
#include <game_object.hpp>
#include <game_objects_loader.hpp>
#include <textures_loader.hpp>

#include <iostream>

const std::string PokerClose::kPokerCloseGameObjects =
    GetAssetPrefix() + "/game_objects/poker_close_objects.json";

void ReturnButtonHandler(StateManager* manager, IGameState* state, nlohmann::json& data) {
    manager->Pop();
}

PokerClose::PokerClose(StateManager* manager)
    : stop_game_thread_(false), run_game_(false), game_exec_thr_(([this] { GameExecutor(); })),
      table_(std::make_unique<PokerTable>(logs_, render_queue_)),
      root_game_object_(ParseGameObjects(kPokerCloseGameObjects)) {
    root_game_object_->Resize(manager->GetWindowSize());
    root_game_object_->AddHandler(sf::Event::MouseButtonPressed, ReturnButtonHandler,
                                  "return_button");
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

void PokerClose::HandleEvent(const sf::Event& event) {
    if (event.type != sf::Event::MouseButtonPressed) {
        return;
    }

    json event_data;
    event_data["event"]["mouse_button"]["x"] = event.mouseButton.x;
    event_data["event"]["mouse_button"]["y"] = event.mouseButton.y;
    event_data["event"]["type"] = event.type;
    root_game_object_->TriggerHandler(&StateManager::Instance(), this, event_data);
}

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
    static std::optional<json> render_event = std::nullopt;
    if (render_event != std::nullopt) {
        std::shared_ptr<GameObject> target_object = root_game_object_->FindGameObjectByTag(
            render_event.value()["tag"].template get<std::string>());
        std::string new_phase = render_event.value()["new_phase"].template get<std::string>();
        uint64_t delay = render_event.value()["delay"].template get<uint64_t>();
        if (target_object->TryUpdatePhase(new_phase, delay)) {
            render_event = std::nullopt;
        }
    } else {
        while (!render_queue_.empty()) {
            if (render_event != std::nullopt) {
                break;
            }
            render_event = render_queue_.pop();
        }
    }

    sf::sleep(sf::milliseconds(50));
    root_game_object_->Draw(manager->GetOriginWindow());
}
