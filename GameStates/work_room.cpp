#include "work_room.h"
#include "SFML/Graphics/Sprite.hpp"
#include "SFML/Window/Event.hpp"
#include "blackjack_close.h"
#include "poker_close.h"
#include "state_manager.h"
#include "textures_loader.h"
#include <game_objects_loader.hpp>

#include <algorithm>
#include <cmath>
#include <memory>

WorkRoomState::WorkRoomState(StateManager* manager) : root_game_object_(std::move(ParseGameObjects(kWorkRoomGameObjects))) {
    
}

void WorkRoomState::HandleEvent(const sf::Event& event) {
    
}

void WorkRoomState::Update(sf::Time delta) {}

void WorkRoomState::Draw(StateManager* manager) {
    for (auto camera : cameras_) {
        manager->DrawSprite(camera);
    }
}
