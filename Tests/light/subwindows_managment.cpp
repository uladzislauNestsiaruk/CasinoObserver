#include <gtest/gtest.h>
#include <memory>

#include "SFML/Graphics/Color.hpp"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Window/WindowStyle.hpp"
#include "subwindows_manager.h"
#include <stats_window.hpp>
/*
TEST(BasicFunctionality, OneStatsWindow) {
    sf::RenderWindow render_window;
    render_window.create(sf::VideoMode::getDesktopMode(), "SubwindowsTests",
                         sf::Style::Resize | sf::Style::Close);

    SubwindowsManager manager(&render_window);
    manager.AddSubwindow(std::make_shared<StatsWindow>(200, 200));
    manager.AddSubwindow(std::make_shared<StatsWindow>(200, 200));

    while (true) {
        render_window.clear();
        manager.Draw();
        bool closed = false;
        sf::Event event;
        while (render_window.pollEvent(event)) {
            manager.HandleEvent(event);
            if (event.type == sf::Event::Closed) {
                closed = true;
                render_window.close();
                break;
            }
        }
        if (closed) {
            break;
        }
        render_window.display();
    }
}*/
