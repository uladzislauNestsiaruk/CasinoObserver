#include <memory>
#include <stdexcept>

#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Window/Event.hpp"
#include "SFML/Window/VideoMode.hpp"
#include "SFML/Window/WindowStyle.hpp"
#include "game_state.hpp"
#include "state_manager.hpp"
#include "work_room.hpp"
#include <textures_loader.hpp>

const size_t kReserveStatesAmount = 32;
const size_t kDefaultFrameLimit = 60;

void StateManager::Init() {
    game_window_.create(sf::VideoMode::getDesktopMode(), kGameTitle,
                        sf::Style::Resize | sf::Style::Close);

    if (!game_window_.isOpen()) {
        throw std::runtime_error("unable to intialize game window");
    }

    Preload();
    game_window_.setFramerateLimit(kDefaultFrameLimit);

    is_running_ = true;
    states_.clear();
    states_.reserve(kReserveStatesAmount);

    states_.emplace_back(std::make_shared<WorkRoomState>(this));
}

void StateManager::Push(std::shared_ptr<IGameState> state) { states_.emplace_back(state); }

void StateManager::Clean() {
    game_window_.close();
    states_.clear();
}

void StateManager::HandleEvents() {
    sf::Event event;
    while (game_window_.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            Quit();
            return;
        }

        states_.back()->HandleEvent(event);
    }
}

std::shared_ptr<IGameState> StateManager::Pop() {
    if (states_.size() <= 1) {
        throw std::runtime_error("attempt to pop state from empty manager stack");
    }

    std::shared_ptr<IGameState> top = states_.back();
    states_.pop_back();
    return top;
}

void StateManager::Update(sf::Time delta) {
    for (auto state : states_) {
        state->Update(delta);
    }
}

void StateManager::Draw() {
    game_window_.clear();
    states_.back()->Draw(this);
    game_window_.display();
}
