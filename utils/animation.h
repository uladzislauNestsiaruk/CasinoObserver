#pragma once

#include "../GameStates/state_manager.h"
#include "SFML/Graphics/Rect.hpp"
#include "SFML/Graphics/Sprite.hpp"
#include "SFML/Graphics/Texture.hpp"
#include "SFML/System/Time.hpp"

class Animation {
    sf::Time frame_switch_time = sf::milliseconds(5); // switch frame each 5 milliseconds
    struct Frame {
        Frame(int32_t frame_h, int32_t frame_w)
            : window(0, 0, frame_h, frame_w), elapsed(sf::Time::Zero) {}

        sf::IntRect window;
        sf::Time elapsed;
    };

public:
    Animation(const sf::Texture& image, size_t images_per_row, size_t images_per_col,
              sf::Time switch_time = sf::milliseconds(5))
        : frame_switch_time(switch_time), sprite_(image), image_(image),
          frame_(image_.getSize().x / images_per_row, image_.getSize().y / images_per_col) {}

    void ChangePosition(float x, float y) { sprite_.setPosition(x, y); }

    bool IsFinished();

    void Update(sf::Time time_elapsed);

    void Draw(StateManager* manager) { manager->DrawSprite(sprite_); }

    void Reset();

private:
    sf::Sprite sprite_;
    const sf::Texture& image_;
    Frame frame_;
};
