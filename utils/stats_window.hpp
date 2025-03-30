#pragma once

#include <algorithm>
#include <animations_manager.hpp>
#include <cstdint>
#include <cstdlib>
#include <memory>
#include <string>
#include <vector>

#include "SFML/Graphics/Texture.hpp"
#include "SFML/System/Vector2.hpp"
#include <../game_object/game_object.hpp>
#include <../game_object/game_objects_loader.hpp>
#include <textures_loader.hpp>

class BasicRow : public GameObject {
public:
    explicit BasicRow(const std::string& background_path, size_t row_id,
                      AnimationsManager& animations_manager)
        : GameObject(background_path + std::to_string(row_id), "afk", animations_manager),
          cells_() {
        AddPhase(std::vector<sf::Sprite>(1, sf::Sprite(GetTexture(background_path))), "afk");
    }

    void AddCell(std::shared_ptr<GameObject> cell) { cells_.emplace_back(cell); }

    virtual const std::string& BackgroundPath() = 0;

    virtual ~BasicRow() {}

private:
    std::vector<std::shared_ptr<GameObject>> cells_;
};

class DefaultRow : public BasicRow {
    static const std::string kBackgroundSpriteTag;

public:
    explicit DefaultRow(size_t row_id, AnimationsManager& animations_manager)
        : BasicRow(kBackgroundSpriteTag, row_id, animations_manager) {}

    const std::string& BackgroundPath() override { return kBackgroundSpriteTag; }

    ~DefaultRow() override {}

private:
};

class StatsWindow : public GameObject {
    static constexpr char kTagPrefix[] = "stats_window";

    friend DEFINE_GOHANDLER(OnMousePressedHandler);
    friend DEFINE_GOHANDLER(OnMouseMoveHandler);
    friend DEFINE_GOHANDLER(OnMouseReleasedHandler);
    friend DEFINE_GOHANDLER(OnMouseScrolledHandler);

public:
    StatsWindow() = delete;

    StatsWindow(const std::string& sprite_code, const std::string& qualifier,
                AnimationsManager& animations_manager);

    void AddRow(std::shared_ptr<BasicRow> row);

    sf::Vector2f GetYBorders() {
        return sf::Vector2f(GetPosition().y + GetSize().y * vert_padding.x,
                            GetPosition().y + GetSize().y * vert_padding.y);
    }

    ~StatsWindow() override {}

private:
    void InitRowPos(size_t ind) {
        data_[ind]->Move(
            {0, GetYBorders().x + GetRowSizeVec().y * ind - data_[ind]->GetPosition().y});
    }

    sf::Vector2f GetRowSizeVec() {
        return sf::Vector2f(GetSize().x,
                            (vert_padding.y - vert_padding.x) * GetSize().y / visible_rows_);
    }

private:
    std::vector<std::shared_ptr<BasicRow>> data_;
    int32_t first_visible_row_;
    bool mouse_pressed_;
    sf::Vector2f mouse_pressed_cords_;
    std::string qualifier_;

    size_t visible_rows_ = 10;
    sf::Vector2f vert_padding = {0.03, 0.98};
    float first_visible_row_y_ = INT16_MAX;
};
