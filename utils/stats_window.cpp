#include <cstdint>
#include <memory>
#include <stdexcept>
#include <type_traits>

#include "SFML/System/Vector2.hpp"
#include "json.hpp"
#include "stats_window.hpp"
#include <game_object.hpp>

template <class Row>
    requires std::is_base_of_v<GameObject, Row>
void StatsWindow<Row>::OnMousePressedHandler(StateManager& manager, IGameState* state,
                                             const nlohmann::json& data) {
    if (!data.contains("x") || !data.contains("y")) {
        throw std::logic_error(
            "not enough info about mouse press event provided to the stats_window");
    }

    mouse_pressed_ = true;
    mouse_pressed_cords =
        sf::Vector2f(data["x"].template get<float>(), data["y"].template get<float>());
}

template <class Row>
    requires std::is_base_of_v<GameObject, Row>
void StatsWindow<Row>::OnMouseReleasedHandler(StateManager& manager, IGameState* state,
                                              const nlohmann::json& data) {
    mouse_pressed_ = false;
    mouse_pressed_cords = sf::Vector2f(0, 0);
}

template <class Row>
    requires std::is_base_of_v<GameObject, Row>
void StatsWindow<Row>::OnMouseMoveHandler(StateManager& manager, IGameState* state,
                                          const nlohmann::json& data) {
    if (!data.contains("x") || !data.contains("y")) {
        throw std::logic_error(
            "not enough info about mouse move event provided to the stats_window");
    }

    sf::Vector2f mouse_position =
        sf::Vector2f(data["x"].template get<int32_t>(), data["y"].template get<int32_t>());
    root_object_->Move(mouse_position - mouse_pressed_cords);
    mouse_pressed_cords = mouse_position;
}

template <class Row>
    requires std::is_base_of_v<GameObject, Row>
void StatsWindow<Row>::OnMouseScrolledHandler(StateManager& manager, IGameState* state,
                                              const nlohmann::json& data) {
    if (visible_rows_ >= data_.size() || mouse_pressed_) {
        return;
    }

    if (!data.contains("delta")) {
        throw std::logic_error(
            "not enough info about mouse scroll event provided to the stats_window");
    }

    float delta = data["delta"].template get<float>();
    sf::Vector2f window_position =
        static_cast<sf::Vector2f>(root_object_->GetPosition());
    sf::Vector2f window_y_borders = sf::Vector2f(
        window_position.y, window_position.y + root_object_->GetSize().y);

    auto prefix_deleted = [&]() {
        ++first_visible_row_;
        Row& row = data[first_visible_row_ + visible_rows_ - 1];
        sf::Vector2f row_pos = static_cast<sf::Vector2f>(row.GetSpriteRect().getPosition());

        row.Move(sf::Vector2f(window_position.x, window_y_borders.y) - row_pos);
        root_object_->AddChild(row);
    };
    auto suffix_deleted = [&]() {
        first_visible_row_ = std::max(0ul, first_visible_row_ - 1);
        Row& row = data[first_visible_row_];
        sf::Vector2f row_pos = static_cast<sf::Vector2f>(row.GetSpriteRect().getPosition());

        row.Move(window_position - row_pos);
        root_object_->AddChild(row);
    };

    for (size_t ind = first_visible_row_;
         ind < min(first_visible_row_ + visible_rows_, data_.size()); ind++) {
         std::shared_ptr<GameObject> row = root_object_->FindGameObjectByTag("row" + std::to_string(ind)); 
        row->Move(sf::Vector2f(0, delta));
        float new_row_y = row->GetPosition().y;
        if (new_row_y > window_y_borders.y || new_row_y < window_y_borders.x) {
            root_object_->RemoveChild("default_phase",
                                      "row" + std::to_string(ind));
            delta > 0 ? prefix_deleted() : suffix_deleted();
        }
    }
}
