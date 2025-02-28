#pragma once

#include <memory>
#include <vector>

#include "SFML/Graphics.hpp"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/System/Vector2.hpp"

const std::string kGameTitle = "Casino observer";

class IGameState;

// I have no idea of situation when it's usefull to have multiple state managers
// in the game, so it would be Singleton
class StateManager {
public:
    void Init();
    void Clean();

    static StateManager& Instance() {
        static StateManager manager;
        return manager;
    }

    StateManager(const StateManager&) = delete;
    StateManager(const StateManager&&) = delete;

    StateManager& operator=(const StateManager&) = delete;
    StateManager& operator=(const StateManager&&) = delete;

    void Push(std::shared_ptr<IGameState> state);
    std::shared_ptr<IGameState> Pop();

    void HandleEvents();
    void Update(sf::Time delta);
    void Draw();

    bool Running() { return is_running_; }

    void Quit() { is_running_ = false; }

    void DrawSprite(const sf::Sprite& sprite) { game_window_.draw(sprite); }

    sf::Vector2u GetWindowSize() { return game_window_.getSize(); }

    sf::RenderWindow* GetOriginWindow() { return &game_window_; }

private:
    std::vector<std::shared_ptr<IGameState>> states_;
    sf::RenderWindow game_window_;
    bool is_running_;

    StateManager() { Init(); }
    ~StateManager() {}
};
