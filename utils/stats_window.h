#pragma once

#include <cstdint>
#include <filesystem>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <vector>

#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/Sprite.hpp"
#include "SFML/Graphics/Text.hpp"
#include "SFML/System/Vector2.hpp"

class BaseStatsEntity {
public:
    virtual void Resize(size_t new_width, size_t new_hight) = 0;

    virtual sf::Vector2i GetSize() = 0;

    virtual void Move(int32_t new_x, int32_t new_y) = 0;

private:
};

class SpriteStatsEntity : public BaseStatsEntity {
public:
    void Resize(size_t new_width, size_t height) override;

    sf::Vector2i GetSize() override { return sprite_.getTextureRect().getSize(); }

    void Move(int32_t new_x, int32_t new_y) override { sprite_.setPosition(new_x, new_y); }

    const sf::Sprite& GetSprite() { return sprite_; }

private:
    sf::Sprite sprite_;
};

class TextStatsEntity : public BaseStatsEntity {
public:
    void Resize(size_t new_width, size_t new_hight) override;

    sf::Vector2i GetSize() override {
        return static_cast<sf::Vector2i>(text_.getGlobalBounds().getSize());
    }

    void Move(int32_t new_x, int32_t new_y) override { text_.setPosition(new_x, new_y); }

    const sf::Text GetText() { return text_; }

private:
    sf::Text text_;
};

namespace StatsWindowUtils {
template <typename T> bool Is(const std::shared_ptr<BaseStatsEntity> entity) {
    return static_cast<bool>(std::dynamic_pointer_cast<T>(entity));
}

template <typename T>
    requires(std::is_same_v<T, TextStatsEntity> || std::is_same_v<T, SpriteStatsEntity>)
std::shared_ptr<T> As(std::shared_ptr<BaseStatsEntity> entity) {
    std::shared_ptr<T> result = std::dynamic_pointer_cast<T>(entity);
    if (!result) {
        throw std::invalid_argument("attempt to cast StatsEntity to another type of StatsEntity");
    }

    return result;
}

}; // namespace StatsWindowUtils

class StatsRow {
public:
    StatsRow() : info_(), size_(), position_() {}

    StatsRow(size_t width, size_t height, size_t pos_x, size_t pos_y,
             const std::vector<std::shared_ptr<BaseStatsEntity>>& info);

    void SetPosition(size_t new_x, size_t new_y) { position_ = sf::Vector2u(new_x, new_y); }

    sf::Vector2u GetPosition() { return position_; }

    void Resize(size_t new_height, size_t new_width);

    void Draw(sf::RenderWindow* window);

private:
    std::vector<std::shared_ptr<BaseStatsEntity>> info_;
    sf::Vector2u size_;
    sf::Vector2u position_;
};

class StatsWindow {
public:
    StatsWindow() : window_(), stats_(nullptr), priority_(0) {}

    StatsWindow(size_t height, size_t width);

    StatsWindow(size_t height, size_t width, size_t pos_x, size_t pos_y,
                std::vector<StatsRow>* stats);

    void PushRow(StatsRow&& row);

    void DeleteRow(size_t ind);

    void Resize(size_t new_width, size_t new_hight);

    void MoveWindow(size_t new_x, size_t new_y);

    void ChangePriority(size_t priority) { priority_ = priority; }

    size_t GetPriority() { return priority_; }

    void Close();

    sf::Vector2f GetSize() { return window_.getSize(); }

    size_t GetRowsAmount() { return stats_->size(); }

    void Draw(sf::RenderWindow* window);

private:
    sf::RectangleShape window_;
    std::vector<StatsRow>* stats_;
    size_t priority_;
    size_t first_visible_row_ = 0;
    size_t visible_rows_ = 10;
};
