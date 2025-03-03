#include <chrono>
#include <filesystem>
#include <iostream>
#include <memory>
#include <stdexcept>

#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/System/Vector2.hpp"
#include "SFML/Window/Event.hpp"
#include "SFML/Window/VideoMode.hpp"
#include "SFML/Window/WindowStyle.hpp"
#include "utils/game_object_manager.hpp"
#include "utils/json.hpp"

#include <game_object.hpp>
#include <game_objects_loader.hpp>
#include <stats_window.hpp>
#include <textures_loader.hpp>

int main(int argc, char** argv) {
    if (argc < 2) {
        throw std::invalid_argument("provide path to the json description of the game object");
    }

    if (!std::filesystem::exists(argv[1])) {
        std::logic_error(std::string("no such file as ./") + argv[1]);
    }

    Preload();
    GOManager objects_manager;
    std::shared_ptr<StatsWindow> stats_window =
        std::make_shared<StatsWindow>("stats_subwindow_subwindow_background", 1);
    for (size_t ind = 0; ind < 20; ind++) {
        stats_window->AddRow(std::make_shared<DefaultRow>(ind + 1));
    }
    objects_manager.AddObject(stats_window);
    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "preview",
                            sf::Style::Resize | sf::Style::Close);
    objects_manager.FindObjectByTag(stats_window->GetTag())->Resize(sf::Vector2f(960, 640));
    sf::Event last_event;

    while (true) {
        window.clear();
        while (window.pollEvent(last_event)) {
            if (last_event.type == sf::Event::Closed) {
                window.close();
                return 0;
            }
            nlohmann::json event;
            event["event"]["type"] = last_event.type;
            event["event"]["x"] = last_event.type == sf::Event::MouseMoved
                                      ? last_event.mouseMove.x
                                      : last_event.mouseButton.x;
            event["event"]["y"] = last_event.type == sf::Event::MouseMoved
                                      ? last_event.mouseMove.y
                                      : last_event.mouseButton.y;
            event["event"]["delta"] = last_event.mouseWheelScroll.delta;

            if (last_event.type == sf::Event::MouseWheelScrolled) {
                event["event"]["x"] = last_event.mouseWheelScroll.x;
                event["event"]["y"] = last_event.mouseWheelScroll.y;
                event["event"]["wheel_id"] = last_event.mouseWheelScroll.wheel;
            }
            objects_manager.HandleEvent(nullptr, event);
        }
        objects_manager.DrawAll(&window);
        window.display();
    }

    return 0;
}
