#pragma once

#include "game_state.hpp"
#include <textures_loader.hpp>

const std::string kInterrogationRoomGameObjects =
    GetAssetPrefix() + "game_objects/interrogation_room_objects.json";

class InterrogationState : public AbstractGameState {
public:
    InterrogationState(std::string_view objects_path, StateManager* manager);

    void HandleEvent(const sf::Event& event) override;
    void Update(sf::Time delta) override;
    void Draw(StateManager* manager) override;

    void SetIsClosed(bool value) { is_closed_ = value; }

private:
    bool is_closed_ = false;
};