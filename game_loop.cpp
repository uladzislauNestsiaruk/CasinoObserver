// "Copyright [2024] Netsiaruk Uladzislau"
#include "SFML/System/Clock.hpp"
#include "state_manager.h"

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
