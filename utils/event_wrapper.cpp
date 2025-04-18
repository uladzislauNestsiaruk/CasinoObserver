#include "event_wrapper.hpp"

void EventWrapper::Wrap(const sf::Event& event, nlohmann::json& data) noexcept {
    data["event"]["type"] = event.type;
    data["event"]["unwrapped"] = 0;

    switch (event.type) {
    case sf::Event::MouseMoved:
        data["event"]["x"] = event.mouseMove.x;
        data["event"]["y"] = event.mouseMove.y;
        break;
    case sf::Event::MouseButtonPressed:
    case sf::Event::MouseButtonReleased:
        data["event"]["x"] = event.mouseButton.x;
        data["event"]["y"] = event.mouseButton.y;
        data["event"]["button"] = event.mouseButton.button;
        break;
    case sf::Event::MouseWheelScrolled:
        data["event"]["x"] = event.mouseWheelScroll.x;
        data["event"]["y"] = event.mouseWheelScroll.y;
        data["event"]["wheel_id"] = event.mouseWheelScroll.wheel;
        data["event"]["delta"] = event.mouseWheelScroll.delta;
        break;
    default:
        data["event"]["unwrapped"] = 1;
        break;
    }
}
