// Copyright [2024] Nestsiarul Uladzislau

#include "poker_close.hpp"
#include "SFML/System/Time.hpp"
#include "SFML/Window/Event.hpp"
#include "game_object.hpp"
#include "state_manager.h"
#include <game_objects_loader.hpp>
#include <textures_loader.hpp>

#include <iostream>

const std::string PokerClose::kPokerCloseGameObjects =
    GetAssetPrefix() + "/game_objects/poker_close_objects.json";

PokerClose::PokerClose(StateManager* manager)
    : stop_game_thread_(false), run_game_(false), game_exec_thr_(([this] { GameExecutor(); })),
      table_(std::make_unique<PokerTable>(logs_, render_queue_)),
      root_game_object_(ParseGameObjects(kPokerCloseGameObjects)){
    root_game_object_->Resize(manager->GetWindowSize());
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
    static std::optional<json> render_event = std::nullopt;
    if (render_event != std::nullopt){
        std::shared_ptr<GameObject> target_object = root_game_object_->FindGameObjectByTag(render_event.value()["tag"].template get<std::string>());
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
