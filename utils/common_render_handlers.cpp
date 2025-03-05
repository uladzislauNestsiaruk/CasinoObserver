#include <iostream>
#include <memory>
#include <string>

#include "common_render_handlers.hpp"
#include <../game_states/table_state.hpp>
#include <../game_object/game_object.hpp>
#include <render_events_manager.hpp>

DEFINE_GOHANDLER(CommonGOEventHandlers::ReturnButtonHandler) {
    manager->Pop();
}

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

    std::cout << "HUY\n";
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
    std::cout << "Deal!\n";
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
    return target_object->TryUpdatePhase(new_phase, delay);
}
