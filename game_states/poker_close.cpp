#include "SFML/System/Time.hpp"
#include "SFML/Window/Event.hpp"

#include "game_state.hpp"
#include "poker_close.hpp"
#include "state_manager.hpp"
#include <common_render_handlers.hpp>
#include <game_object.hpp>
#include <textures_loader.hpp>

#include <chrono>
#include <iostream>
#include <thread>

const std::string PokerClose::kPokerCloseGameObjects =
    GetAssetPrefix() + "game_objects/poker_close_objects.json";

PokerClose::PokerClose(StateManager* manager)
    : AbstractGameState(kPokerCloseGameObjects), stop_game_thread_(false), run_game_(false),
      game_exec_thr_(([this] { GameExecutor(); })) {
    table_ = std::make_unique<PokerTable>(logs_, render_events_manager_.GetRenderQueue());
    std::shared_ptr<GameObject> root_game_object_ = objects_manager_.FindObjectByTag("root");
    root_game_object_->Resize(manager->GetWindowSize());
    for (const auto& place : kGamblersPlaces) {
        for (const auto& person_tag : place) {
            root_game_object_->AddHandler(sf::Event::MouseButtonPressed,
                                          CommonGOEventHandlers::PlayerHandler, person_tag);
        }
    }

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
            std::cout << "checking2\n";
            table_->ClearRollback();
            run_game_.store(false);
            table_->GameIteration();
        }
    }
}

void PokerClose::HandleEvent(const sf::Event& event) {
    if (event.type != sf::Event::MouseButtonPressed) {
        return;
    }

    json event_data;
    event_data["event"]["x"] = event.mouseButton.x;
    event_data["event"]["y"] = event.mouseButton.y;
    event_data["event"]["type"] = event.type;
    objects_manager_.HandleEvent(this, event_data);
    std::cout << "after handler\n";
}

void PokerClose::Update(sf::Time delta) {
    std::cout << table_->IsGameFinished() << ' ' << logs_.empty() << ' '
              << render_events_manager_.IsEmpty() << '\n';
    if (table_->IsGameFinished() && logs_.empty() && render_events_manager_.IsEmpty()) {
        run_game_.store(true);
    }

    render_events_manager_.HandleEvent();
    while (!logs_.empty()) {
        std::optional<json> log_event = logs_.pop();
        if (log_event == std::nullopt) {
            continue;
        }

        // std::cout << "[LOG_EVENT]:\n" << log_event << '\n';
    }
}

void PokerClose::Draw(StateManager* manager) {
    sf::sleep(sf::milliseconds(50));
    objects_manager_.DrawAll(manager->GetOriginWindow());
}

void PokerClose::BanPlayers() {
    table_->SetWasActionPerformed(true);
    while (!table_->IsGameFinished()) {
        std::cout << "checking game finished\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    std::cout << "before rollback\n";
    table_->RollbackGame();
    std::cout << "after rollback\n";
    // auto start = std::chrono::steady_clock::now();
    // std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    // auto end = std::chrono::steady_clock::now();

    // std::cout << "Slept for "
    //           << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
    //           << " ms\n";
    std::cout << "GOAAAL2\n";
    std::cout << "Ban players!!!\n";
    render_events_manager_.Clear();
    while (!logs_.empty()) {
        logs_.pop();
    }
    for (const auto& person_tag : selected_players_) {
        std::cout << person_tag << '\n';
        table_->RemovePlayer(person_tag);
    }
    std::cout << "Here: " << render_events_manager_.GetSize() << "\n";
    selected_players_.clear();
    table_->SetWasActionPerformed(false);
    for (const std::string& card_id : {"first", "second", "third", "fourth", "fivth"}) {
        json render_event;
        render_event["event"]["type"] = "change_phase";
        render_event["new_phase"] = "empty";
        render_event["tag"] = card_id + "_central_card";
        render_event["delay"] = 0;

        objects_manager_.FindObjectByTag(card_id + "_central_card")->FinishPhase();
        table_->AddRenderEvent(render_event);
    }
    std::cout << "Here: " << render_events_manager_.GetSize() << "\n";
}
