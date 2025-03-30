#include <common.hpp>
#include <constants.hpp>

#include "event_wrapper.hpp"
#include "interrogation_state.hpp"
#include "state_manager.hpp"
#include "table_state.hpp"

namespace {
std::vector<std::string> GetTableCardObjectsTags(GameType game_type) {
    std::vector<std::string> tags;
    if (game_type == GameType::Poker) {
        for (const std::string& card_id : {"first", "second", "third", "fourth", "fivth"}) {
            tags.push_back(card_id + "_central_card");
        }
    } else {
        for (size_t player_id = 1; player_id <= 6; ++player_id) {
            for (size_t card_num = 1; card_num <= 2; ++card_num) {
                tags.push_back("player_" + std::to_string(player_id) + "_" +
                               std::to_string(card_num));
            }
        }

        tags.push_back("dealer_1");
    }

    return tags;
}
} // namespace

TableState::TableState(const std::string& objects_path, StateManager* manager,
                       table_creater_t table_creater)
    : AbstractGameState(objects_path), stop_game_thread_(false), run_game_(false),
      game_exec_thr_(([this] { GameExecutor(); })) {
    table_ = table_creater(logs_, render_events_manager_.GetRenderQueue(), animations_mananger_);
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

void TableState::HandleEvent(const sf::Event& event) {
    json event_data;
    EventWrapper::Wrap(event, event_data);
    objects_manager_.HandleEvent(this, event_data, true);
}

void TableState::Update(sf::Time delta) {
    if (table_->IsGameFinished() && logs_.empty() && render_events_manager_.IsEmpty()) {
        run_game_.store(true);
    }

    render_events_manager_.HandleEvent();
    while (!logs_.empty()) {
        std::optional<json> log_event = logs_.pop();
        if (log_event == std::nullopt) {
            continue;
        }
    }
}

void TableState::Draw(StateManager* manager) {
    sf::sleep(sf::milliseconds(50));
    objects_manager_.DrawAll(manager->GetOriginWindow());
}

void TableState::GameExecutor() {
    while (!stop_game_thread_) {
        if (run_game_.load()) {
            table_->ClearRollback();
            run_game_.store(false);
            table_->GameIteration();
        }
    }
}

TableState::~TableState() {
    stop_game_thread_.store(true);
    game_exec_thr_.join();
}

void TableState::BanPlayers() {
    table_->SetWasActionPerformed(true);
    while (!table_->IsGameFinished()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    table_->RollbackGame();
    render_events_manager_.Clear();
    while (!logs_.empty()) {
        logs_.pop();
    }

    for (const auto& person_tag : selected_players_) {
        table_->RemovePlayer(person_tag);
    }

    selected_players_.clear();
    table_->SetWasActionPerformed(false);
    for (const std::string& card_tag : GetTableCardObjectsTags(table_->GetGameType())) {
        json render_event;
        render_event["event"]["type"] = "change_phase";
        render_event["new_phase"] = "empty";
        render_event["tag"] = card_tag;
        render_event["delay"] = 0;

        objects_manager_.FindObjectByTag(card_tag)->FinishPhase();
        table_->AddRenderEvent(render_event);
    }

    for (const auto& player : table_->GetPlayers()) {
        table_->AddRenderEvent({{"event", {{"type", "change_phase"}}},
                                {"new_phase", "chips_" + std::to_string(player->GetBalanceLevel())},
                                {"tag", ExtractPersonPlace(player->GetPersonTag()) + "_chips"},
                                {"delay", 0}});
    }
}

void TableState::DealPlayers(StateManager* manager) {
    BanPlayers();
    manager->Push(std::make_shared<InterrogationState>(kInterrogationRoomGameObjects, manager));
}
