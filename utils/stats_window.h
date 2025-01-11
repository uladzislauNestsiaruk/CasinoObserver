#pragma once

#include <cstdint>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <vector>

#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/Sprite.hpp"
#include "SFML/Graphics/Text.hpp"
#include "SFML/System/Vector2.hpp"
#include "subwindows_manager.h"

class BaseStatsEntity {
public:
    virtual void Resize(float new_width, float new_hight) = 0;

    virtual sf::Vector2f GetSize() = 0;

    virtual void Move(float new_x, float new_y) = 0;

    virtual ~BaseStatsEntity() {}

private:
};

class SpriteStatsEntity : public BaseStatsEntity {
public:
    void Resize(float new_width, float height) override;

    sf::Vector2f GetSize() override {
        return static_cast<sf::Vector2f>(sprite_.getTextureRect().getSize());
    }

    void Move(float new_x, float new_y) override { sprite_.setPosition(new_x, new_y); }

    const sf::Sprite& GetSprite() { return sprite_; }

    ~SpriteStatsEntity() override {}

private:
    sf::Sprite sprite_;
};

class TextStatsEntity : public BaseStatsEntity {
public:
    void Resize(float new_width, float new_hight) override;

    sf::Vector2f GetSize() override { return text_.getGlobalBounds().getSize(); }

    void Move(float new_x, float new_y) override { text_.setPosition(new_x, new_y); }

    const sf::Text GetText() { return text_; }

    ~TextStatsEntity() override {}

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

    StatsRow(float width, float height, float pos_x, float pos_y,
             const std::vector<std::shared_ptr<BaseStatsEntity>>& info);

    void SetPosition(float new_x, float new_y) { position_ = sf::Vector2f(new_x, new_y); }

    sf::Vector2f GetPosition() { return position_; }

    void Resize(float new_height, float new_width);

    void Draw(sf::RenderWindow* window);

private:
    std::vector<std::shared_ptr<BaseStatsEntity>> info_;
    sf::Vector2f size_;
    sf::Vector2f position_;
};

class StatsWindow : public Subwindow {
public:
    StatsWindow() : Subwindow(), stats_(nullptr) {}

    StatsWindow(float height, float width);

    StatsWindow(float height, float width, float pos_x, float pos_y, std::vector<StatsRow>* stats);

    void Resize(float new_width, float new_hight) override;

    void MoveWindow(float new_x, float new_y) override;

    void Close() override;

    void Draw(sf::RenderWindow* window) override;

    void PushRow(StatsRow&& row);

    void DeleteRow(size_t ind);

    size_t GetRowsAmount() { return stats_->size(); }

    ~StatsWindow() override {}

private:
    std::vector<StatsRow>* stats_;
    size_t first_visible_row_ = 0;
    size_t visible_rows_ = 10;
};
