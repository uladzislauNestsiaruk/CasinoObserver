#include <iostream>
#include <memory>
#include <string>

#include "common_render_handlers.hpp"
#include <../GameStates/poker_close.hpp>
#include <game_object.hpp>
#include <render_events_manager.hpp>

void CommonGOEventHandlers::ReturnButtonHandler(StateManager* manager, IGameState* state,
                                                nlohmann::json& data) {
    manager->Pop();
}

bool CommonREMEventHandlers::ChangePhaseHandler(RenderEventsManager<json>* render_manager,
                                                const json& data) {
    std::cout << "Call: " << data["event_type"].template get<std::string>() << ' '
              << data["tag"].template get<std::string>()
              << data["new_phase"].template get<std::string>() << '\n';
    std::shared_ptr<GameObject> target_object =
        static_cast<const AbstractGameState*>(render_manager->GetState())
            ->FindGameObjectByTag(data["tag"].template get<std::string>());
    std::string new_phase = data["new_phase"].template get<std::string>();
    std::cout << target_object->active_phase_ << ' ' << target_object->tag_ << ' ' << new_phase
              << '\n';
    uint64_t delay = data["delay"].template get<uint64_t>();
    return target_object->TryUpdatePhase(new_phase, delay);
}
