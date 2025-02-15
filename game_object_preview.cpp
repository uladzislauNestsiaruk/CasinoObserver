#include "GameStates/state_manager.h"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/Sprite.hpp"
#include "SFML/Window/Event.hpp"
#include "SFML/Window/VideoMode.hpp"
#include "SFML/Window/WindowStyle.hpp"
#include "Utils/game_object.hpp"
#include "game_objects_loader.hpp"
#include <filesystem>
#include <memory>
#include <stdexcept>

int main(int argc, char** argv) {
    if (argc < 2) {
        throw std::invalid_argument("provide path to the json description of the game object");
    }

    if (!std::filesystem::exists(argv[1])) {
        std::logic_error(std::string("no such file as ./") + argv[1]);
    }

    StateManager& manager = StateManager::Instance();
    std::unique_ptr<GameObject> object = ParseGameObjects(argv[1]);
    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "preview",
                            sf::Style::Resize | sf::Style::Close);
    object->Resize(window.getSize());
    sf::Event last_event;

    while (true) {
        window.clear();

        while (window.pollEvent(last_event)) {
            if (last_event.type == sf::Event::Closed) {
                window.close();
                return 0;
            }
        }
        object->Draw(window);
        window.display();
    }

    return 0;
}
