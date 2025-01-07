#include "work_room.h"
#include "../Tables/poker_table.h"
#include "../state_manager.h"
#include "SFML/Graphics/Sprite.hpp"
#include "SFML/Window/Event.hpp"
#include "blackjack_close.h"
#include "poker_close.h"
#include "textures_loader.h"

#include <algorithm>
#include <cmath>
#include <memory>

WorkRoomState::WorkRoomState(StateManager* manager) {
    auto window_center =
        sf::Vector2(manager->GetWindowSize().x / 2.0, manager->GetWindowSize().y / 2.0);
    int32_t table_width = std::max(1, static_cast<int>(std::sqrt(total_tables)));
    auto camera_size = GetTextute("blackjack_dealed_state").getSize();

    for (size_t ind = 0; ind < total_tables; ind++) {
        if (ind & 1) {
            tables_.push_back(std::make_shared<BlackjackClose>(manager));
            cameras_.emplace_back(GetTextute("blackjack_dealed_state"));
        } else {
            tables_.push_back(std::make_shared<PokerClose>(manager));
            cameras_.emplace_back(GetTextute("poker_dealed_state"));
        }

        cameras_[ind].setPosition(
            window_center.x + camera_size.x * static_cast<int32_t>(ind / table_width),
            window_center.y + camera_size.y * static_cast<int32_t>(ind % table_width));
    }
}

void WorkRoomState::HandleEvent(const sf::Event& event) {
    if (event.type != sf::Event::MouseButtonPressed) {
        return;
    }

    sf::Vector2 up_left_cords = cameras_[0].getPosition();
    sf::Vector2 down_right_cords = cameras_.back().getPosition();
    sf::Vector2 event_cords = sf::Vector2(event.mouseButton.x, event.mouseButton.y);
    if (event_cords.x < up_left_cords.x || event_cords.y < up_left_cords.y ||
        event_cords.x > down_right_cords.x || event_cords.y > down_right_cords.y) {
        return;
    }

    sf::Vector2 camera_size = GetTextute("blackjack_dealed_state").getSize();
    int32_t grid_width = std::max(1, static_cast<int>(std::sqrt(total_tables)));

    size_t table_ind =
        static_cast<int32_t>((event_cords.y - up_left_cords.y) / camera_size.y) * grid_width +
        static_cast<int32_t>((event_cords.x - up_left_cords.x) / camera_size.x);
    StateManager& manager = StateManager::Instance();
    manager.Push(tables_[table_ind]);
}

void WorkRoomState::Update(sf::Time delta) {}

void WorkRoomState::Draw(StateManager* manager) {
    for (auto camera : cameras_) {
        manager->DrawSprite(camera);
    }
}
