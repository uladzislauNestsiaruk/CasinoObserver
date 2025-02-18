#include <memory>

#include "common_render_handlers.hpp"
#include <../GameStates/poker_close.hpp>
#include <game_object.hpp>
#include <render_events_manager.hpp>

void CommonGOEventHandlers::ReturnButtonHandler(StateManager* manager, IGameState* state,
                                                nlohmann::json& data) {
    manager->Pop();
}

bool CommonREMEventHandlers::ChangePhaseHandler(RenderEventsManager<json>* render_manager, const json& data) {
    std::shared_ptr<GameObject> target_object =
        static_cast<const AbstractGameState*>(render_manager->GetState())
            ->FindGameObjectByTag(data["tag"].template get<std::string>());
    std::string new_phase = data["new_phase"].template get<std::string>();
    uint64_t delay = data["delay"].template get<uint64_t>();
    return target_object->TryUpdatePhase(new_phase, delay);
}
