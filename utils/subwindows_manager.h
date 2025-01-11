#pragma once

#include <cstddef>
#include <list>
#include <memory>
#include <unordered_map>

#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/System/Vector2.hpp"
#include "SFML/Window/Event.hpp"

class Subwindow {
public:
    Subwindow() : window_() {}

    Subwindow(float width, float height, size_t priority = 0)
        : window_(sf::Vector2f(width, height)) {}

    Subwindow(float width, float height, float new_x, float new_y, size_t priority = 0)
        : window_(sf::Vector2f(width, height)) {
        window_.setPosition(new_x, new_y);
    }

    Subwindow(const Subwindow& subwindow) : window_(subwindow.window_) {}

    Subwindow(Subwindow&& subwindow) : window_(std::move(subwindow.window_)) {}

    Subwindow& operator=(const Subwindow& subwindow) {
        if (this != &subwindow) {
            window_ = subwindow.window_;
        }

        return *this;
    }

    Subwindow& operator=(Subwindow&& subwindow) {
        if (this != &subwindow) {
            window_ = std::move(subwindow.window_);
        }

        return *this;
    }

    virtual void Resize(float new_width, float new_height) = 0;

    virtual void MoveWindow(float new_x, float new_y) = 0;

    virtual void Close() = 0;

    virtual void Draw(sf::RenderWindow* window) = 0;

    virtual ~Subwindow() {}

    sf::Vector2f GetSize() noexcept { return window_.getSize(); }

    void SetSize(sf::Vector2f size) {
        window_.setScale(size.x / window_.getSize().x, size.y / window_.getSize().y);
    }

    void SetPosition(float new_x, float new_y) { window_.setPosition(sf::Vector2f(new_x, new_y)); }

    sf::Vector2f GetPosition() noexcept { return window_.getPosition(); }

    bool Contains(float x, float y) noexcept { return window_.getGlobalBounds().contains(x, y); }

    const sf::RectangleShape& GetWindow() noexcept { return window_; }

private:
    sf::RectangleShape window_;
};

class SubwindowsManager {
public:
    SubwindowsManager() : window_(nullptr), subwindows_() {}

    explicit SubwindowsManager(sf::RenderWindow* window)
        : window_(window), subwindows_() {}

    SubwindowsManager(sf::RenderWindow* window,
                      const std::list<std::shared_ptr<Subwindow>>& subwindows)
        : SubwindowsManager(window) {
        subwindows_ = subwindows;
    }

    void AddSubwindow(std::shared_ptr<Subwindow> subwindow);

    void Draw();

    void HandleEvent(const sf::Event& event);

private:
    sf::RenderWindow* window_;
    std::list<std::shared_ptr<Subwindow>> subwindows_;
    bool mouse_pressed_ = false;

    void MouseButtonPressed(const sf::Event& event);
    void MouseButtonReleased(const sf::Event& event);
    void MouseMoved(const sf::Event& event);

    std::unordered_map<sf::Event::EventType, void (*)(const sf::Event&)> kSubwindowsEvents;
};
