#include "interrogation_state.hpp"
#include "state_manager.hpp"
#include <common_render_handlers.hpp>

InterrogationState::InterrogationState(std::string_view objects_path, StateManager* manager)
    : AbstractGameState(objects_path) {
    std::shared_ptr<GameObject> root_game_object_ = objects_manager_.FindObjectByTag("root");
    root_game_object_->Resize(manager->GetWindowSize());
    root_game_object_->AddHandler(sf::Event::MouseButtonPressed,
                                  CommonGOEventHandlers::ReturnHandler, "root");
}

void InterrogationState::HandleEvent(const sf::Event& event) {
    if (event.type != sf::Event::MouseButtonPressed) {
        return;
    }

    json event_data;
    event_data["event"]["x"] = event.mouseButton.x;
    event_data["event"]["y"] = event.mouseButton.y;
    event_data["event"]["type"] = event.type;
    objects_manager_.HandleEvent(this, event_data);
}

void InterrogationState::Update(sf::Time delta) {
    if (is_closed_) {
        StateManager::Instance().Pop();
    }
}

void InterrogationState::Draw(StateManager* manager) {
    objects_manager_.DrawAll(manager->GetOriginWindow());
}