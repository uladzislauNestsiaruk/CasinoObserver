#include <memory>

#include "SFML/Window/Event.hpp"
#include "subwindows_manager.h"

void SubwindowsManager::AddSubwindow(std::shared_ptr<Subwindow> subwindow) {
    subwindows_.emplace_back(subwindow);
}

void SubwindowsManager::Draw() {
    for (auto subwindow_ptr : subwindows_) {
        subwindow_ptr->Draw(window_);
    }
}

void SubwindowsManager::HandleEvent(const sf::Event& event) {
    if (kSubwindowsEvents.find(event.type) == kSubwindowsEvents.end()) {
        return;
    }

    kSubwindowsEvents.at(event.type)(event);
}

void SubwindowsManager::MouseButtonPressed(const sf::Event& event) {
    if (subwindows_.empty()) {
        return;
    }

    mouse_pressed_ = true;
}

void SubwindowsManager::MouseButtonReleased(const sf::Event& event) { mouse_pressed_ = false; }

void SubwindowsManager::MouseMoved(const sf::Event& event) {
    if (mouse_pressed_) {
        subwindows_.back()->MoveWindow(event.mouseMove.x, event.mouseMove.y);
        return;
    }

    if (!subwindows_.empty() &&
        subwindows_.back()->Contains(event.mouseMove.x, event.mouseMove.y)) {
        return;
    }

    for (std::list<std::shared_ptr<Subwindow>>::reverse_iterator subwindow_it =
             subwindows_.rbegin();
         subwindow_it != subwindows_.rend(); ++subwindow_it) {
        if ((*subwindow_it)->Contains(event.mouseMove.x, event.mouseMove.y)) {
            subwindows_.emplace_back(*subwindow_it);
            subwindows_.erase(--(subwindow_it.base()));
            break;
        }
    }
}
