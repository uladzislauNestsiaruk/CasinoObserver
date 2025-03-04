#pragma once

#include <memory>
#include <vector>

#include "SFML/Graphics.hpp"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/System/Vector2.hpp"

constexpr char kGameTitle[] = "Casino observer";

class IGameState;

////////////////////////
//////  StateManager responsible for game states(the same as scenes) managment.
//////
//////  Game states organized in a stack, in that way, that only top most state would be displayed
/////   on
//////  the screen.
//////
//////   Event handling would be done only for the top most state.
//////
//////   Update would be called for all states in the stack.
//////
//////   StateManager is singleton class, you can access it via Instance() method
////////////////////////

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

    sf::Vector2f GetWindowSize() { return static_cast<sf::Vector2f>(game_window_.getSize()); }

    sf::RenderWindow* GetOriginWindow() { return &game_window_; }

private:
    std::vector<std::shared_ptr<IGameState>> states_;
    sf::RenderWindow game_window_;
    bool is_running_;

    StateManager() { Init(); }
    ~StateManager() {}
};
