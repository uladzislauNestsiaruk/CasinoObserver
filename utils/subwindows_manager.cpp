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

    kSubwindowsEvents.at(event.type)(this, event);
}

void MouseButtonPressed(SubwindowsManager* manager, const sf::Event& event) {
    if (manager->subwindows_.empty()) {
        return;
    }

    manager->mouse_pressed_ = true;
}

void MouseButtonReleased(SubwindowsManager* manager, const sf::Event& event) {
    manager->mouse_pressed_ = false;
}

void MouseMoved(SubwindowsManager* manager, const sf::Event& event) {
    if (manager->mouse_pressed_) {
        manager->subwindows_.back()->MoveWindow(event.mouseMove.x, event.mouseMove.y);
        return;
    }

    if (!manager->subwindows_.empty() &&
        manager->subwindows_.back()->Contains(event.mouseMove.x, event.mouseMove.y)) {
        return;
    }

    for (std::list<std::shared_ptr<Subwindow>>::reverse_iterator subwindow_it =
             manager->subwindows_.rbegin();
         subwindow_it != manager->subwindows_.rend(); ++subwindow_it) {
        if ((*subwindow_it)->Contains(event.mouseMove.x, event.mouseMove.y)) {
            manager->subwindows_.emplace_back(*subwindow_it);
            manager->subwindows_.erase(--(subwindow_it.base()));
            break;
        }
    }
}
