#pragma once

#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <type_traits>
#include <vector>

#include "SFML/Graphics/Rect.hpp"
#include "SFML/System/Vector2.hpp"
#include "json.hpp"
#include <game_object.hpp>
#include <game_objects_loader.hpp>
#include <textures_loader.hpp>

template <typename Row>
    requires std::is_base_of_v<GameObject, Row>
DEFINE_GOHANDLER(OnMouseMoveHandler);

template <typename Row>
    requires std::is_base_of_v<GameObject, Row>
DEFINE_GOHANDLER(OnMousePressedHandler);

template <typename Row>
    requires std::is_base_of_v<GameObject, Row>
DEFINE_GOHANDLER(OnMouseReleasedHandler);

template <typename Row>
    requires std::is_base_of_v<GameObject, Row>
DEFINE_GOHANDLER(OnMouseScrolledHandler);

class DefaultRow : public GameObject {
    static constexpr char kBackgroundSpriteTag[] = "stats_subwindow_row_background";
    static constexpr char kRowPrefix[] = "row";

public:
    DefaultRow(size_t row_id)
        : GameObject(kRowPrefix + std::to_string(row_id), "afk",
                     std::vector<sf::Sprite>(1, sf::Sprite(GetTextute(kBackgroundSpriteTag)))),
          cells_() {}

    void AddCell(std::shared_ptr<GameObject> cell) { cells_.emplace_back(cell); }

    ~DefaultRow() override {}

private:
    std::vector<std::shared_ptr<GameObject>> cells_;
};

template <class Row = DefaultRow>
    requires std::is_base_of_v<GameObject, Row>
class StatsWindow : public GameObject {
    static constexpr char kTagPrefix[] = "stats_window";

    friend DEFINE_GOHANDLER(OnMousePressedHandler<Row>);
    friend DEFINE_GOHANDLER(OnMouseMoveHandler<Row>);
    friend DEFINE_GOHANDLER(OnMouseReleasedHandler<Row>);
    friend DEFINE_GOHANDLER(OnMouseScrolledHandler<Row>);

public:
    StatsWindow() = delete;

    explicit StatsWindow(const std::string& sprite_code, size_t window_id)
        : window_id_(window_id),
          GameObject(kTagPrefix + std::to_string(window_id), "afk",
                     std::vector<sf::Sprite>(1, sf::Sprite(GetTextute(sprite_code)))),
          data_(), first_visible_row_(0), mouse_pressed_(false) {
        AddHandler(sf::Event::MouseMoved, OnMouseMoveHandler<Row>, GetTag());
        AddHandler(sf::Event::MouseButtonPressed, OnMousePressedHandler<Row>, GetTag());
        AddHandler(sf::Event::MouseButtonReleased, OnMouseReleasedHandler<Row>, GetTag());
        AddHandler(sf::Event::MouseWheelScrolled, OnMouseScrolledHandler<Row>, GetTag());
    }

    void AddRow(std::shared_ptr<Row> row) {
        row->Resize(GetRowSizeVec());
        row->Move(sf::Vector2f(GetPosition().x - row->GetPosition().x, 0));
        data_.emplace_back(row);

        if (data_.size() <= visible_rows_) {
            AddChild(row, "afk");
            InitRowPos(data_.size() - 1);
        } else {
            row->Move(sf::Vector2f(-1, 0));
        }
    }

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
    std::vector<std::shared_ptr<Row>> data_;
    int32_t first_visible_row_;
    bool mouse_pressed_;
    sf::Vector2f mouse_pressed_cords_;
    size_t window_id_;

    size_t visible_rows_ = 10;
    sf::Vector2f vert_padding = {0.03, 0.98};
};

template <typename Row>
    requires std::is_base_of_v<GameObject, Row>
void OnMousePressedHandler(StateManager* manager, IGameState* state, GameObject* object,
                           nlohmann::json& data) {
    StatsWindow<Row>* casted_object = static_cast<StatsWindow<Row>*>(object);
    casted_object->mouse_pressed_ = true;
    casted_object->mouse_pressed_cords_ = sf::Vector2f(data["event"]["x"].template get<float>(),
                                                       data["event"]["y"].template get<float>());
}

template <class Row>
    requires std::is_base_of_v<GameObject, Row>
void OnMouseReleasedHandler(StateManager* manager, IGameState* state, GameObject* object,
                            nlohmann::json& data) {
    StatsWindow<Row>* casted_object = static_cast<StatsWindow<Row>*>(object);
    casted_object->mouse_pressed_ = false;
    casted_object->mouse_pressed_cords_ = sf::Vector2f(0, 0);
}

template <typename Row>
    requires std::is_base_of_v<GameObject, Row>
void OnMouseMoveHandler(StateManager* manager, IGameState* state, GameObject* object,
                        nlohmann::json& data) {
    StatsWindow<Row>* casted_object = static_cast<StatsWindow<Row>*>(object);
    if (!casted_object->mouse_pressed_) {
        return;
    }

    sf::Vector2f mouse_position = sf::Vector2f(data["event"]["x"].template get<float>(),
                                               data["event"]["y"].template get<float>());

    object->Move(mouse_position - casted_object->mouse_pressed_cords_);
    casted_object->mouse_pressed_cords_ = mouse_position;
}

template <typename Row>
void UpdateRowSubRect(GameObject* row, StatsWindow<Row>* stats_window, sf::Vector2f row_size_vec) {
    sf::Vector2f row_pos = row->GetPosition();
    sf::IntRect original_rect =
        sf::Sprite(GetTextute("stats_subwindow_row_background")).getTextureRect();
    sf::Vector2f window_y_borders = stats_window->GetYBorders();
    float row_height =
        fmin(row_pos.y + row_size_vec.y, window_y_borders.y) - fmax(row_pos.y, window_y_borders.x);
    float inv_prefix_delta = 2 * fmax(0.0, window_y_borders.x - row_pos.y);
    original_rect.top = inv_prefix_delta;
    std::cout << row->GetTag() << " " << original_rect.top << "\n";
    original_rect.height *= row_height / row_size_vec.y;

    row->SetVisibleRect(original_rect);
}

template <class Row>
    requires std::is_base_of_v<GameObject, Row>
void OnMouseScrolledHandler(StateManager* manager, IGameState* state, GameObject* object,
                            nlohmann::json& data) {
    // check is scroll event tagged on right wheel, without it some strange stuff can happen
    // on touchpads
    if (!data["event"].contains("wheel_id") || data["event"]["wheel_id"].get<int32_t>()) {
        return;
    }

    // no sense to scroll window all rows already seen
    StatsWindow<Row>* casted_object = static_cast<StatsWindow<Row>*>(object);
    if (casted_object->visible_rows_ >= casted_object->data_.size() ||
        casted_object->mouse_pressed_) {
        return;
    }

    sf::Vector2f window_position = casted_object->GetPosition();
    sf::Vector2f window_y_borders = casted_object->GetYBorders();
    sf::Vector2f row_size_vec = casted_object->GetRowSizeVec();

    float delta = data["event"]["delta"].get<float>();
    float max_delta = window_y_borders.x -
                      casted_object->data_[casted_object->first_visible_row_]->GetPosition().y +
                      casted_object->first_visible_row_ * row_size_vec.y;
    float min_delta =
        window_y_borders.y -
        casted_object->data_[casted_object->first_visible_row_]->GetPosition().y -
        row_size_vec.y * (casted_object->data_.size() - casted_object->first_visible_row_);
    delta = fmin(delta, max_delta);
    delta = fmax(delta, min_delta);

    if (delta == 0) {
        return;
    }

    size_t last_updated_row = 0;
    for (size_t ind = casted_object->first_visible_row_; ind < casted_object->data_.size(); ind++) {
        std::shared_ptr<GameObject> row = casted_object->data_[ind];
        if (row->GetPosition().x != window_position.x) {
            break;
        }
        last_updated_row = ind;

        row->Move(sf::Vector2f(0, delta));
        float new_row_y = row->GetPosition().y;
        if (new_row_y >= window_y_borders.y || new_row_y + row_size_vec.y <= window_y_borders.x) {
            casted_object->RemoveChild("afk", row->GetTag());
            row->Move(sf::Vector2f(-1, 0));
            continue;
        }

        UpdateRowSubRect<Row>(row.get(), casted_object, row_size_vec);
    }

    while (casted_object->first_visible_row_ > 0 && delta > 0) {
        sf::Vector2f row_pos =
            casted_object->data_[casted_object->first_visible_row_]->GetPosition();
        if (row_pos.y <= window_y_borders.x) {
            break;
        }

        std::shared_ptr<GameObject> row =
            casted_object->data_[casted_object->first_visible_row_ - 1];
        row->Resize(row_size_vec);
        row->Move(sf::Vector2f(window_position.x - row->GetPosition().x,
                               row_pos.y - row_size_vec.y - row->GetPosition().y));
        UpdateRowSubRect<Row>(row.get(), casted_object, row_size_vec);
        casted_object->AddChild(row, "afk");
        --casted_object->first_visible_row_;
    }

    while (casted_object->data_[casted_object->first_visible_row_]->GetPosition().x !=
               window_position.x &&
           delta < 0) {
        ++casted_object->first_visible_row_;
    }

    while (last_updated_row < casted_object->data_.size() - 1 && delta < 0) {
        std::shared_ptr<GameObject> prev_row = casted_object->data_[last_updated_row];
        sf::Vector2f prev_row_pos = prev_row->GetPosition();
        if (prev_row_pos.y + row_size_vec.y >= window_y_borders.y) {
            break;
        }

        std::shared_ptr<GameObject> row = casted_object->data_[last_updated_row + 1];
        row->Resize(row_size_vec);
        row->Move(sf::Vector2f(window_position.x - row->GetPosition().x,
                               prev_row_pos.y + row_size_vec.y - row->GetPosition().y));
        UpdateRowSubRect<Row>(row.get(), casted_object, row_size_vec);
        casted_object->AddChild(row, "afk");
        ++last_updated_row;
    }
}
