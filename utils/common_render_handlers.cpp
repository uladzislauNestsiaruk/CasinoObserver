#include <iostream>
#include <memory>
#include <string>

#include "common_render_handlers.hpp"
#include <../game_states/poker_close.hpp>
#include <game_object.hpp>
#include <render_events_manager.hpp>

void CommonGOEventHandlers::ReturnButtonHandler(StateManager* manager, IGameState* state,
                                                GameObject* object, const nlohmann::json& data) {
    manager->Pop();
}

void CommonGOEventHandlers::SelectButtonHandler(StateManager* manager, IGameState* state,
                                                GameObject* object, const nlohmann::json& data) {
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
    // позже вынести общее двух столов в AbstractTableState и поменять здесь каст
    static_cast<PokerClose*>(state)->SetIsSelectPressed(true);
}

void CommonGOEventHandlers::BanButtonHandler(StateManager* manager, IGameState* state,
                                             GameObject* object, const json& data) {
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
    PokerClose* poker_close = static_cast<PokerClose*>(state);
    if (poker_close->SizeSelectPlayer()) {
        static_cast<PokerClose*>(state)->SetIsSelectPressed(false);
        select_button_object->TryUpdatePhase("afk", delay);
        ban_button_object->TryUpdatePhase("empty", delay);
        deal_button_object->TryUpdatePhase("empty", delay);
        poker_close->BanPlayers();
    }
}

void CommonGOEventHandlers::DealButtonHandler(StateManager* manager, IGameState* state,
                                              GameObject* object, const json& data) {
    std::cout << "Deal!\n";
}

void CommonGOEventHandlers::PlayerHandler(StateManager* manager, IGameState* state,
                                          GameObject* object, const json& data) {
    bool is_select_pressed = static_cast<PokerClose*>(state)->GetIsSelectPressed();
    if (is_select_pressed) {
        PokerClose* poker_close = static_cast<PokerClose*>(state);
        if (!poker_close->ContainsSelectPlayer(object->GetTag())) {
            std::cout << "ADDDDDDD: " << poker_close->SizeSelectPlayer() << "\n";
            poker_close->AddSelectPlayer(object->GetTag());
        } else {
            std::cout << "ERASE: " << poker_close->SizeSelectPlayer() << "\n";
            poker_close->EraseSelectPlayer(object->GetTag());
        }

        return;
    }

    // Handle stats window
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
