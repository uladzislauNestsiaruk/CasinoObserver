#include "work_room.h"
#include "SFML/Graphics/Sprite.hpp"
#include "SFML/System/Vector2.hpp"
#include "SFML/Window/Event.hpp"
#include "state_manager.h"
#include <game_objects_loader.hpp>

WorkRoomState::WorkRoomState(StateManager* manager)
    : root_game_object_(std::move(ParseGameObjects(kWorkRoomGameObjects))) {}

void WorkRoomState::HandleEvent(const sf::Event& event) {
    if (event.type != sf::Event::MouseButtonPressed) {
        return;
    }

    root_game_object_->TriggerHandler(event);
}

void WorkRoomState::Update(sf::Time delta) {}

void WorkRoomState::Draw(StateManager* manager) {
    root_game_object_->Draw(manager->GetOriginWindow());
}
