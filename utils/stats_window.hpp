#pragma once

#include <memory>
#include <type_traits>
#include <vector>

#include <game_object.hpp>

class DefaultRow : public GameObject {};

template <class Row = DefaultRow>
    requires std::is_base_of_v<GameObject, Row>
class StatsWindow : public GameObject {
    static constexpr size_t kDefaultVisibleRows = 10;

public:
    StatsWindow() = delete;

    explicit StatsWindow(const std::vector<Row>& data)
        : data_(data), first_visible_row_(0), mouse_pressed_(false) {}

    void AddRow(const Row& row) { data_.emplace_back(row); }

    ~StatsWindow() {}

private:
    std::vector<Row> data_;
    size_t visible_rows_ = kDefaultVisibleRows;
    size_t first_visible_row_;
    bool mouse_pressed_;
    sf::Vector2f mouse_pressed_cords;

    DEFINE_GOHANDLER(OnMousePressedHandler);
    DEFINE_GOHANDLER(OnMouseMoveHandler);
    DEFINE_GOHANDLER(OnMouseReleasedHandler);
    DEFINE_GOHANDLER(OnMouseScrolledHandler);
};
