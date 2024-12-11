#include "SFML/Graphics.hpp"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Window/Event.hpp"
#include "SFML/Window/VideoMode.hpp"
#include "SFML/Window/Window.hpp"
#include "SFML/Window/WindowStyle.hpp"
#include "textures_loader.h"

void Draw(sf::RenderWindow &window) {
    window.clear();
    window.draw(sf::Sprite(GetTextute("blackjack_table")));
    window.display();
}

int main() {
    sf::RenderWindow game_window(sf::VideoMode::getDesktopMode(),
                                 "Casino Observer",
                                 sf::Style::Close | sf::Style::Resize);

    Preload();

    while (game_window.isOpen()) {
        sf::Event event;
        while (game_window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                game_window.close();
            }
        }

        Draw(game_window);
    }

    return 0;
}
