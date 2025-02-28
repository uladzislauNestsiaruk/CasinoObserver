#include "SFML/System/Time.hpp"
#include "SFML/Window/Event.hpp"

#include "game_state.hpp"
#include "poker_close.hpp"
#include "state_manager.hpp"
#include <common_render_handlers.hpp>
#include <game_object.hpp>
#include <textures_loader.hpp>

#include <iostream>

const std::string PokerClose::kPokerCloseGameObjects =
    GetAssetPrefix() + "game_objects/poker_close_objects.json";

PokerClose::PokerClose(StateManager* manager)
    : AbstractGameState(kPokerCloseGameObjects), stop_game_thread_(false), run_game_(false),
      game_exec_thr_(([this] { GameExecutor(); })) {
    table_ = std::make_unique<PokerTable>(logs_, render_events_manager_.GetRenderQueue());
    std::shared_ptr<GameObject> root_game_object_ = objects_manager_.FindObjectByTag("root");
    root_game_object_->Resize(manager->GetWindowSize());
    root_game_object_->AddHandler(sf::Event::MouseButtonPressed,
                                  CommonGOEventHandlers::ReturnButtonHandler, "return_button");
    root_game_object_->AddHandler(sf::Event::MouseButtonPressed,
                                  CommonGOEventHandlers::SelectButtonHandler, "select_button");
    root_game_object_->AddHandler(sf::Event::MouseButtonPressed,
                                  CommonGOEventHandlers::DealButtonHandler, "deal_button");
    root_game_object_->AddHandler(sf::Event::MouseButtonPressed,
                                  CommonGOEventHandlers::BanButtonHandler, "ban_button");

    render_events_manager_.AddHandler("change_phase", CommonREMEventHandlers::ChangePhaseHandler);
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
    objects_manager_.HandleEvent(this, event_data);
}

void PokerClose::Update(sf::Time delta) {
    if (table_->IsGameFinished() && logs_.empty() && render_events_manager_.IsEmpty()) {
        run_game_.store(true);
    }

    render_events_manager_.HandleEvent();
    while (!logs_.empty()) {
        std::optional<json> log_event = logs_.pop();
        if (log_event == std::nullopt) {
            continue;
        }

        std::cout << "[LOG_EVENT]:\n" << log_event << '\n';
    }
}

void PokerClose::Draw(StateManager* manager) {
    sf::sleep(sf::milliseconds(50));
    objects_manager_.DrawAll(manager->GetOriginWindow());
}
