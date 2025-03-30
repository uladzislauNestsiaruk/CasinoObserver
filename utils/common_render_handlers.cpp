#include <iostream>
#include <memory>
#include <optional>
#include <string>

#include "SFML/System/Vector2.hpp"
#include "common_render_handlers.hpp"
#include <../game_object/game_object.hpp>
#include <../game_states/interrogation_state.hpp>
#include <../game_states/table_state.hpp>
#include <render_events_manager.hpp>
#include <stats_window.hpp>

DEFINE_GOHANDLER(CommonGOEventHandlers::ReturnButtonHandler) { manager->Pop(); }

DEFINE_GOHANDLER(CommonGOEventHandlers::SelectButtonHandler) {
    std::shared_ptr<GameObject> select_button_object =
        static_cast<AbstractGameState*>(state)->FindGameObjectByTag("select_button");
    std::shared_ptr<GameObject> ban_button_object =
        static_cast<AbstractGameState*>(state)->FindGameObjectByTag("ban_button");
    std::shared_ptr<GameObject> deal_button_object =
        static_cast<AbstractGameState*>(state)->FindGameObjectByTag("deal_button");

    uint64_t delay = 0;
    if (data.contains("delay")) {
        delay = data["delay"].template get<uint64_t>();
    }
    if (select_button_object->GetActivePhase() == "empty") {
        return;
    }

    select_button_object->TryUpdatePhase("empty", delay);
    ban_button_object->TryUpdatePhase("afk", delay);
    deal_button_object->TryUpdatePhase("afk", delay);
    static_cast<TableState*>(state)->SetIsSelectPressed(true);
}

DEFINE_GOHANDLER(CommonGOEventHandlers::BanButtonHandler) {
    std::shared_ptr<GameObject> select_button_object =
        static_cast<AbstractGameState*>(state)->FindGameObjectByTag("select_button");
    std::shared_ptr<GameObject> ban_button_object =
        static_cast<AbstractGameState*>(state)->FindGameObjectByTag("ban_button");
    std::shared_ptr<GameObject> deal_button_object =
        static_cast<AbstractGameState*>(state)->FindGameObjectByTag("deal_button");

    uint64_t delay = 0;
    if (data.contains("delay")) {
        delay = data["delay"].template get<uint64_t>();
    }

    TableState* table_state = static_cast<TableState*>(state);
    if (table_state->SizeSelectPlayer()) {
        table_state->SetIsSelectPressed(false);
        select_button_object->TryUpdatePhase("afk", delay);
        ban_button_object->TryUpdatePhase("empty", delay);
        deal_button_object->TryUpdatePhase("empty", delay);
        table_state->BanPlayers();
    }
}

DEFINE_GOHANDLER(CommonGOEventHandlers::DealButtonHandler) {
    std::shared_ptr<GameObject> select_button_object =
        static_cast<AbstractGameState*>(state)->FindGameObjectByTag("select_button");
    std::shared_ptr<GameObject> ban_button_object =
        static_cast<AbstractGameState*>(state)->FindGameObjectByTag("ban_button");
    std::shared_ptr<GameObject> deal_button_object =
        static_cast<AbstractGameState*>(state)->FindGameObjectByTag("deal_button");

    uint64_t delay = 0;
    if (data.contains("delay")) {
        delay = data["delay"].template get<uint64_t>();
    }

    TableState* table_state = static_cast<TableState*>(state);
    if (table_state->SizeSelectPlayer()) {
        table_state->SetIsSelectPressed(false);
        select_button_object->TryUpdatePhase("afk", delay, std::nullopt);
        ban_button_object->TryUpdatePhase("empty", delay, std::nullopt);
        deal_button_object->TryUpdatePhase("empty", delay, std::nullopt);
        table_state->DealPlayers(manager);
    }
}

DEFINE_GOHANDLER(CommonGOEventHandlers::PlayerHandler) {
    TableState* table_state = static_cast<TableState*>(state);
    bool is_select_pressed = table_state->GetIsSelectPressed();
    if (is_select_pressed) {
        if (!table_state->ContainsSelectPlayer(object->GetTag())) {
            std::cout << "ADDDDDDD: " << table_state->SizeSelectPlayer() << "\n";
            table_state->AddSelectPlayer(object->GetTag());
        } else {
            std::cout << "ERASE: " << table_state->SizeSelectPlayer() << "\n";
            table_state->EraseSelectPlayer(object->GetTag());
        }

        return;
    } else {
        std::shared_ptr<StatsWindow> player_statistics =
            std::make_shared<StatsWindow>("stats_subwindow_subwindow_background", object->GetTag(),
                                          table_state->GetAnimationsManager());

        if (table_state->FindGameObjectByTag(player_statistics->GetTag())) {
            return;
        }
        std::shared_ptr<GameObject> root = table_state->FindGameObjectByTag("root");
        player_statistics->Resize(sf::Vector2f(root->GetSize().x / 4, root->GetSize().y / 2),
                                  false);
        table_state->AddIndependentObject(player_statistics);
    }
}

bool CommonREMEventHandlers::ChangePhaseHandler(RenderEventsManager<json>* render_manager,
                                                const json& data) {
    std::shared_ptr<GameObject> target_object =
        static_cast<const AbstractGameState*>(render_manager->GetState())
            ->FindGameObjectByTag(data["tag"].template get<std::string>());
    if (target_object == nullptr) {
        throw std::logic_error("Trying to find GameObject with tag \"" +
                               data["tag"].template get<std::string>() + "\"");
    }
    std::string new_phase = data["new_phase"].template get<std::string>();
    uint64_t delay = data["delay"].template get<uint64_t>();
    std::optional<uint64_t> new_animation_id;
    if (data.contains("new_animation_id")) {
        new_animation_id = data["new_animation_id"].template get<uint64_t>();
    }
    std::optional<uint64_t> after_animation_id;
    if (data.contains("after_animation_id")) {
        after_animation_id = data["after_animation_id"].template get<uint64_t>();
    }
    return target_object->TryUpdatePhase(new_phase, delay, new_animation_id, after_animation_id);
}

DEFINE_GOHANDLER(CommonGOEventHandlers::ReturnHandler) {
    static_cast<InterrogationState*>(state)->SetIsClosed(true);
}
