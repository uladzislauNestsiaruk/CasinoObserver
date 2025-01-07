#include "animation.h"
#include "SFML/Graphics/Rect.hpp"
#include "SFML/Graphics/Sprite.hpp"
#include "SFML/System/Vector2.hpp"

bool Animation::IsFinished() {
    sf::Vector2u image_rect = image_.getSize();
    return frame_.window.left + frame_.window.width >= image_rect.x &&
           frame_.window.top + frame_.window.height >= image_rect.y;
}

void Animation::Update(sf::Time time_elapsed) {
    frame_.elapsed += time_elapsed;
    if (frame_.elapsed < frame_switch_time) {
        return;
    }

    sf::Vector2u image_rect = image_.getSize();
    sprite_.setTextureRect(sf::IntRect(0, 0, image_rect.x, image_rect.y));
    sf::FloatRect sprite_rect = sprite_.getGlobalBounds();

    if (IsFinished()) {
        return;
    }

    frame_.elapsed -= frame_switch_time;
    frame_.window.left += frame_.window.width;

    if (frame_.window.left >= sprite_rect.left + sprite_rect.width) {
        frame_.window.left = 0;
        frame_.window.top += frame_.window.height;
    }

    sprite_.setTextureRect(frame_.window);
}

void Animation::Reset() {
    frame_.elapsed = sf::Time::Zero;
    frame_.window = sf::Rect(0, 0, frame_.window.width, frame_.window.height);
}
