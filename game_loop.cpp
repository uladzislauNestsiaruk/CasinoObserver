// "Copyright [2024] Netsiaruk Uladzislau"
#include "state_manager.h"


int main() {
    StateManager& manager = StateManager::Instance();

    while (manager.Running()) {
        manager.HandleEvents();
        manager.Update();
        manager.Draw();
    }

    manager.Clean();
    return 0;
}
