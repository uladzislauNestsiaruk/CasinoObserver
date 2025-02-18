#include "../GameStates/poker_close.hpp"
#include "render_events_manager.hpp"

bool ChangePhaseHandler(RenderEventsManager<json>* render_manager, const json& data) {
    std::shared_ptr<GameObject> target_object =
        static_cast<const PokerClose*>(render_manager->GetState())
            ->FindGameObjectByTag(data["tag"].template get<std::string>());
    std::string new_phase = data["new_phase"].template get<std::string>();
    uint64_t delay = data["delay"].template get<uint64_t>();
    return target_object->TryUpdatePhase(new_phase, delay);
}