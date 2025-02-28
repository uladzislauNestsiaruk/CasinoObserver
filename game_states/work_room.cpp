#include <cstdlib>
#include <memory>

#include "SFML/Window/Event.hpp"
#include "blackjack_close.hpp"
#include "game_state.hpp"
#include "poker_close.hpp"
#include "state_manager.hpp"
#include "work_room.hpp"

#include <game_objects_loader.hpp>
#include <textures_loader.hpp>

using nlohmann::json;

const std::string WorkRoomState::kWorkRoomGameObjects =
    GetAssetPrefix() + "game_objects/work_room_objects.json";

size_t ExtractTableId(const std::string& tag) {
    if (!tag.starts_with("screen")) {
        throw std::logic_error(
            "attempt to extract id if the screen from not screen game object in work room scene");
    }

    return atoll(tag.data() + strlen("screen"));
}

void BlackjackScreenHandler(StateManager* manager, IGameState* state, json& data) {
    manager->Push(dynamic_cast<WorkRoomState*>(state)->tables_[ExtractTableId(data["tag"])]);
}

void PokerScreenHandler(StateManager* manager, IGameState* state, json& data) {
    manager->Push(dynamic_cast<WorkRoomState*>(state)->tables_[ExtractTableId(data["tag"])]);
}

WorkRoomState::WorkRoomState(StateManager* manager) : AbstractGameState(kWorkRoomGameObjects) {
    tables_.emplace_back(std::make_shared<BlackjackClose>(manager));
    tables_.emplace_back(std::make_shared<PokerClose>(manager));
    std::shared_ptr<GameObject> root_game_object_ = objects_manager_.FindObjectByTag("root");

    root_game_object_->Resize(manager->GetWindowSize());
    root_game_object_->AddHandler(sf::Event::MouseButtonPressed, BlackjackScreenHandler, "screen0");
    root_game_object_->AddHandler(sf::Event::MouseButtonPressed, PokerScreenHandler, "screen1");
}

void WorkRoomState::HandleEvent(const sf::Event& event) {
    if (event.type != sf::Event::MouseButtonPressed) {
        return;
    }

    json event_data;
    event_data["event"]["mouse_button"]["x"] = event.mouseButton.x;
    event_data["event"]["mouse_button"]["y"] = event.mouseButton.y;
    event_data["event"]["type"] = event.type;
    objects_manager_.HandleEvent(this, event_data);
}

void WorkRoomState::Update(sf::Time delta) {}

void WorkRoomState::Draw(StateManager* manager) {
    objects_manager_.DrawAll(manager->GetOriginWindow());
}
