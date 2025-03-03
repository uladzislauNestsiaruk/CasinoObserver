#include "SFML/System/Clock.hpp"
#include "game_states/state_manager.hpp"

int main() {
    StateManager& manager = StateManager::Instance();
    sf::Clock main_clock = sf::Clock();

    while (manager.Running()) {
        manager.HandleEvents();
        manager.Update(main_clock.restart());
        manager.Draw();
    }

    manager.Clean();
    return 0;
}
