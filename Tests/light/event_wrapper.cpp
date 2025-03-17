#include <event_wrapper.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <algorithm>
#include <array>

#include "SFML/Window/Event.hpp"
#include "SFML/Window/Mouse.hpp"

using wrapped_event = nlohmann::json;

sf::Event::EventType unwrapped_events[] = {sf::Event::Closed,
                                           sf::Event::Resized,
                                           sf::Event::LostFocus,
                                           sf::Event::GainedFocus,
                                           sf::Event::TextEntered,
                                           sf::Event::KeyPressed,
                                           sf::Event::KeyReleased,
                                           sf::Event::MouseWheelMoved,
                                           sf::Event::MouseEntered,
                                           sf::Event::MouseLeft,
                                           sf::Event::JoystickButtonPressed,
                                           sf::Event::JoystickButtonReleased,
                                           sf::Event::JoystickMoved,
                                           sf::Event::JoystickConnected,
                                           sf::Event::JoystickDisconnected,
                                           sf::Event::TouchBegan,
                                           sf::Event::TouchMoved,
                                           sf::Event::TouchEnded,
                                           sf::Event::SensorChanged};
constexpr size_t unwrapped_events_cnt = sizeof(unwrapped_events) / sizeof(sf::Event::EventType);

TEST(EventWrapper, MouseClickEvent) {
    wrapped_event data;

    sf::Event::MouseButtonEvent click_event;
    click_event.button = sf::Mouse::Button::Left;
    click_event.x = 525252;
    click_event.y = 52525252;

    for (sf::Event::EventType test_event_type :
         {sf::Event::MouseButtonPressed, sf::Event::MouseButtonReleased}) {
        sf::Event test_event;
        test_event.type = test_event_type;
        test_event.mouseButton = click_event;

        EventWrapper::Wrap(test_event, data);

        ASSERT_EQ(data["event"]["type"].get<int>(), test_event_type);
        ASSERT_EQ(data["event"]["button"].get<int>(), sf::Mouse::Button::Left);
        ASSERT_EQ(data["event"]["x"].get<int>(), click_event.x);
        ASSERT_EQ(data["event"]["y"].get<int>(), click_event.y);
    }
}

TEST(EventWrapper, MouseMoveEvent) {
    wrapped_event data;

    sf::Event::MouseMoveEvent move_event;
    move_event.x = 525252;
    move_event.y = 52525252;

    sf::Event test_event;
    test_event.type = sf::Event::MouseMoved;
    test_event.mouseMove = move_event;

    EventWrapper::Wrap(test_event, data);

    ASSERT_EQ(data["event"]["type"].get<int>(), sf::Event::MouseMoved);
    ASSERT_EQ(data["event"]["x"].get<int>(), move_event.x);
    ASSERT_EQ(data["event"]["y"].get<int>(), move_event.y);
}

TEST(EventWrapper, WheelScrollEvent) {
    wrapped_event data;

    sf::Event::MouseWheelScrollEvent scroll_event;
    scroll_event.delta = 52;
    scroll_event.wheel = sf::Mouse::Wheel::HorizontalWheel;
    scroll_event.x = 525252;
    scroll_event.y = 52525252;

    sf::Event test_event;
    test_event.type = sf::Event::MouseWheelScrolled;
    test_event.mouseWheelScroll = scroll_event;

    EventWrapper::Wrap(test_event, data);

    ASSERT_EQ(data["event"]["type"].get<int>(), sf::Event::MouseWheelScrolled);
    ASSERT_EQ(data["event"]["wheel_id"].get<int>(), sf::Mouse::Wheel::HorizontalWheel);
    ASSERT_EQ(data["event"]["delta"].get<int>(), scroll_event.delta);
    ASSERT_EQ(data["event"]["x"].get<int>(), scroll_event.x);
    ASSERT_EQ(data["event"]["y"].get<int>(), scroll_event.y);
}

TEST(EventWrapper, CheckIsWrapperProvided) {
    wrapped_event data;
    sf::Event test_event;

    for (size_t event_type = 0; event_type < sf::Event::EventType::Count - 1; event_type++) {
        test_event.type = sf::Event::EventType(event_type);
        EventWrapper::Wrap(test_event, data);

        bool is_unwrapped = data["event"]["unwrapped"].get<int>();
        auto unwrapped_list_begin = std::begin(unwrapped_events);
        auto unwrapped_list_end = std::end(unwrapped_events);
        auto unwrapped_list_pos = std::find(unwrapped_list_begin, unwrapped_list_end, event_type);
        if (is_unwrapped) {
            ASSERT_EQ(event_type, *unwrapped_list_pos);
        } else {
            ASSERT_EQ(unwrapped_list_pos, unwrapped_list_end);
        }
    }
}
