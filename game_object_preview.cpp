#include "SFML/Graphics/RenderWindow.hpp"
#include <filesystem>
#include <memory>
#include <stdexcept>

#include "SFML/Window/Event.hpp"
#include "SFML/Window/VideoMode.hpp"
#include "SFML/Window/WindowStyle.hpp"
#include <game_object.hpp>
#include <game_objects_loader.hpp>

#include <textures_loader.hpp>

int main(int argc, char** argv) {
    if (argc < 2) {
        throw std::invalid_argument("provide path to the json description of the game object");
    }

    if (!std::filesystem::exists(argv[1])) {
        std::logic_error(std::string("no such file as ./") + argv[1]);
    }

    Preload();
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
        object->Draw(&window);
        window.display();
    }

    return 0;
}
