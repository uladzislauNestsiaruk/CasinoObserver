#include "stats_window.hpp"
#include "SFML/Graphics/Rect.hpp"
#include "SFML/System/Vector2.hpp"
#include <../game_object/game_object.hpp>

#include <optional>

DEFINE_GOHANDLER(OnMouseMoveHandler);
DEFINE_GOHANDLER(OnMousePressedHandler);
DEFINE_GOHANDLER(OnMouseReleasedHandler);
DEFINE_GOHANDLER(OnMouseScrolledHandler);

const std::string DefaultRow::kBackgroundSpriteTag = "stats_subwindow_row_background";

StatsWindow::StatsWindow(const std::string& sprite_code, const std::string& qualifier,
                         AnimationsManager& animations_manager)
    : qualifier_(qualifier), GameObject(kTagPrefix + qualifier, "afk", animations_manager), data_(),
      first_visible_row_(0), mouse_pressed_(false) {
    AddPhase(std::vector<sf::Sprite>(1, sf::Sprite(GetTexture(sprite_code))), "afk");
    AddHandler(sf::Event::MouseMoved, OnMouseMoveHandler, GetTag());
    AddHandler(sf::Event::MouseButtonPressed, OnMousePressedHandler, GetTag());
    AddHandler(sf::Event::MouseButtonReleased, OnMouseReleasedHandler, GetTag());
    AddHandler(sf::Event::MouseWheelScrolled, OnMouseScrolledHandler, GetTag());
}

void StatsWindow::AddRow(std::shared_ptr<BasicRow> row) {
    row->Resize(GetRowSizeVec());
    row->SetScale(row->GetScale());
    row->Move(sf::Vector2f(GetPosition().x - row->GetPosition().x, 0));
    data_.emplace_back(row);

    if (data_.size() <= visible_rows_) {
        AddChild(row, "afk");
        InitRowPos(data_.size() - 1);
        first_visible_row_y_ = fmin(first_visible_row_y_, row->GetPosition().y);
    } else {
        row->Move(sf::Vector2f(-1, 0));
    }
}

DEFINE_GOHANDLER(OnMousePressedHandler) {
    StatsWindow* casted_object = static_cast<StatsWindow*>(object);
    casted_object->mouse_pressed_ = true;
    casted_object->mouse_pressed_cords_ = sf::Vector2f(data["event"]["x"].template get<float>(),
                                                       data["event"]["y"].template get<float>());
}

DEFINE_GOHANDLER(OnMouseReleasedHandler) {
    StatsWindow* casted_object = static_cast<StatsWindow*>(object);
    casted_object->mouse_pressed_ = false;
    casted_object->mouse_pressed_cords_ = sf::Vector2f(0, 0);
}

DEFINE_GOHANDLER(OnMouseMoveHandler) {
    StatsWindow* casted_object = static_cast<StatsWindow*>(object);
    if (!casted_object->mouse_pressed_) {
        return;
    }

    sf::Vector2f mouse_position = sf::Vector2f(data["event"]["x"].template get<float>(),
                                               data["event"]["y"].template get<float>());

    object->Move(mouse_position - casted_object->mouse_pressed_cords_);
    casted_object->first_visible_row_y_ += mouse_position.y - casted_object->mouse_pressed_cords_.y;
    casted_object->mouse_pressed_cords_ = mouse_position;
}

void UpdateRowSubRect(BasicRow* row, StatsWindow* stats_window, sf::Vector2f row_size_vec,
                      sf::IntRect original_row_rect) {
    sf::Vector2f row_pos = row->GetPosition();
    sf::Vector2f window_y_borders = stats_window->GetYBorders();
    float row_height =
        fmin(row_pos.y + row_size_vec.y, window_y_borders.y) - fmax(row_pos.y, window_y_borders.x);
    float inv_prefix_delta = fmax(0.0, window_y_borders.x - row_pos.y);
    original_row_rect.top = inv_prefix_delta;
    original_row_rect.height *= row_height / row_size_vec.y;

    row->SetVisibleRect(original_row_rect);
}

DEFINE_GOHANDLER(OnMouseScrolledHandler) {
    // check is scroll event tagged on right wheel, without it some strange stuff can happen
    // on touchpads
    if (!data["event"].contains("wheel_id") || data["event"]["wheel_id"].get<int32_t>()) {
        return;
    }

    // no sense to scroll window all rows already seen
    StatsWindow* casted_object = static_cast<StatsWindow*>(object);
    if (casted_object->visible_rows_ >= casted_object->data_.size() ||
        casted_object->mouse_pressed_) {
        return;
    }

    sf::Vector2f window_position = casted_object->GetPosition();
    sf::Vector2f window_y_borders = casted_object->GetYBorders();
    sf::Vector2f row_size_vec = casted_object->GetRowSizeVec();

    float delta = data["event"]["delta"].get<float>();
    float max_delta = window_y_borders.x - casted_object->first_visible_row_y_ +
                      casted_object->first_visible_row_ * row_size_vec.y;
    float min_delta =
        window_y_borders.y - casted_object->first_visible_row_y_ -
        row_size_vec.y * (casted_object->data_.size() - casted_object->first_visible_row_);
    delta = fmin(delta, max_delta);
    delta = fmax(delta, min_delta);

    if (delta == 0) {
        return;
    }
    casted_object->data_[casted_object->first_visible_row_]->Move(sf::Vector2f(
        0, casted_object->first_visible_row_y_ -
               casted_object->data_[casted_object->first_visible_row_]->GetPosition().y));

    size_t last_updated_row = 0;
    for (size_t ind = casted_object->first_visible_row_; ind < casted_object->data_.size(); ind++) {
        std::shared_ptr<BasicRow> row = casted_object->data_[ind];
        if (row->GetPosition().x != window_position.x) {
            break;
        }
        last_updated_row = ind;

        row->Move(sf::Vector2f(0, delta));
        float new_row_y = row->GetPosition().y;
        if (new_row_y >= window_y_borders.y || new_row_y + row_size_vec.y <= window_y_borders.x) {
            casted_object->RemoveChild(row->GetTag(), std::nullopt);
            row->Move(sf::Vector2f(-1, 0));
            continue;
        }

        UpdateRowSubRect(row.get(), casted_object, row_size_vec,
                         sf::Sprite(GetTexture(row->BackgroundPath())).getTextureRect());
    }

    while (casted_object->first_visible_row_ > 0 && delta > 0) {
        sf::Vector2f row_pos =
            casted_object->data_[casted_object->first_visible_row_]->GetPosition();
        if (row_pos.y <= window_y_borders.x) {
            break;
        }

        std::shared_ptr<BasicRow> row = casted_object->data_[casted_object->first_visible_row_ - 1];
        row->Resize(row_size_vec);
        row->Move(sf::Vector2f(window_position.x - row->GetPosition().x,
                               row_pos.y - row_size_vec.y - row->GetPosition().y));
        UpdateRowSubRect(row.get(), casted_object, row_size_vec,
                         sf::Sprite(GetTexture(row->BackgroundPath())).getTextureRect());
        casted_object->AddChild(row, "afk");
        --casted_object->first_visible_row_;
    }

    while (casted_object->data_[casted_object->first_visible_row_]->GetPosition().x !=
               window_position.x &&
           delta < 0) {
        ++casted_object->first_visible_row_;
    }
    int32_t first_visible_row = casted_object->first_visible_row_;

    while (last_updated_row < casted_object->data_.size() - 1 && delta < 0) {
        std::shared_ptr<GameObject> prev_row = casted_object->data_[last_updated_row];
        sf::Vector2f prev_row_pos = prev_row->GetPosition();
        if (prev_row_pos.y + row_size_vec.y >= window_y_borders.y) {
            break;
        }

        std::shared_ptr<BasicRow> row = casted_object->data_[last_updated_row + 1];
        row->Resize(row_size_vec);
        row->Move(sf::Vector2f(window_position.x - row->GetPosition().x,
                               prev_row_pos.y + row_size_vec.y - row->GetPosition().y));
        UpdateRowSubRect(row.get(), casted_object, row_size_vec,
                         sf::Sprite(GetTexture(row->BackgroundPath())).getTextureRect());
        casted_object->AddChild(row, "afk");
        ++last_updated_row;
    }

    casted_object->first_visible_row_y_ = casted_object->data_[first_visible_row]->GetPosition().y;
    casted_object->data_[first_visible_row]->Move(
        sf::Vector2f(0, window_y_borders.x - casted_object->first_visible_row_y_));
}
