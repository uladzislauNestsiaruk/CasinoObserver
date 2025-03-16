#include "table_state.hpp"
#include "event_wrapper.hpp"

TableState::TableState(const std::string& objects_path, StateManager* manager,
                       table_creater_t table_creater)
    : AbstractGameState(objects_path), stop_game_thread_(false), run_game_(false),
      game_exec_thr_(([this] { GameExecutor(); })) {
    table_ = table_creater(logs_, render_events_manager_.GetRenderQueue());
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
            selected_players_.clear();
            table_->GameIteration();
            run_game_.store(false);
        }
    }
}

TableState::~TableState() {
    stop_game_thread_.store(true);
    game_exec_thr_.join();
}

void TableState::BanPlayers() {
    table_->SetWasActionPerformed(true);
    while (table_->GetWasActionPerformed()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

    for (const auto& person_tag : selected_players_) {
        table_->RemovePlayer(person_tag);
    }
    selected_players_.clear();
}
