#include <algorithm>
#include <cassert>
#include <memory>
#include <stdexcept>
#include <sys/stat.h>

#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/System/Vector2.hpp"
#include "stats_window.h"
#include "subwindows_manager.h"

using namespace StatsWindowUtils;

void SpriteStatsEntity::Resize(float new_width, float new_height) {
    sf::Vector2f entity_size = static_cast<sf::Vector2f>(sprite_.getTextureRect().getSize());
    sf::Vector2f scale_factor = sf::Vector2f(new_width / entity_size.x, new_height / entity_size.y);
    if (scale_factor.x != 1.0 || scale_factor.y != 1.0) {
        sprite_.setScale(scale_factor.x, scale_factor.y);
    }
}

void TextStatsEntity::Resize(float new_width, float new_height) {
    sf::Vector2f entity_size = text_.getGlobalBounds().getSize();
    sf::Vector2f scale_factor = sf::Vector2f(new_width / entity_size.x, new_height / entity_size.y);
    if (scale_factor.x != 1.0 || scale_factor.y != 1.0) {
        text_.setScale(scale_factor.x, scale_factor.y);
    }
}

StatsRow::StatsRow(float width, float height, float pos_x, float pos_y,
                   const std::vector<std::shared_ptr<BaseStatsEntity>>& info)
    : info_(info), size_(width, height), position_(pos_x, pos_y) {
    Resize(width, height);
}

void StatsRow::Resize(float new_height, float new_width) {
    for (auto entity_ptr : info_) {
        if (!entity_ptr) {
            throw std::invalid_argument("passed null StatsEntity");
        }
        entity_ptr->Resize(new_width, new_height);
    }
}

void StatsRow::Draw(sf::RenderWindow* window) {
    sf::Vector2f start_position = info_[0]->GetSize();
    size_t delta_x = 0;
    for (auto entity : info_) {
        entity->Move(start_position.x + delta_x, start_position.y);

        if (Is<SpriteStatsEntity>(entity)) {
            window->draw(As<SpriteStatsEntity>(entity)->GetSprite());
        } else {
            window->draw(As<TextStatsEntity>(entity)->GetText());
        }

        delta_x += entity->GetSize().x;
    }
}

StatsWindow::StatsWindow(float width, float height) : Subwindow(width, height) {}

StatsWindow::StatsWindow(float width, float height, float pos_x, float pos_y,
                         std::vector<StatsRow>* stats)
    : Subwindow(width, height, pos_x, pos_y), stats_(stats) {
    assert(visible_rows_ != 0);
    if (!height || !width || stats->empty()) {
        throw std::invalid_argument("attempt to create stats window with zero size");
    }

    sf::Vector2f row_size = sf::Vector2f(width / static_cast<float>(visible_rows_),
                                         height / static_cast<float>(visible_rows_));

    for (auto& row : *stats) {
        row.Resize(row_size.x, row_size.y);
        row.SetPosition(pos_x, pos_y);
        pos_y += row_size.y;
    }
}

void StatsWindow::PushRow(StatsRow&& row) { stats_->emplace_back(row); }

void StatsWindow::DeleteRow(size_t ind) {
    if (stats_->size() <= ind) {
        throw std::invalid_argument("attempt to delete non existing row in stats window");
    }
    stats_->erase(stats_->begin() + ind);
}

void StatsWindow::Resize(float new_width, float new_height) {
    sf::Vector2f new_row_size = sf::Vector2f(new_width / static_cast<float>(visible_rows_),
                                             new_height / static_cast<float>(visible_rows_));

    for (size_t ind = first_visible_row_;
         ind < std::min(first_visible_row_ + visible_rows_, GetRowsAmount()); ind++) {
        (*stats_)[ind].Resize(new_row_size.x, new_row_size.y);
    }
}

void StatsWindow::MoveWindow(float new_x, float new_y) {
    for (size_t ind = first_visible_row_;
         ind < std::min(first_visible_row_ + visible_rows_, GetRowsAmount()); ind++) {
        sf::Vector2f current_position = (*stats_)[ind].GetPosition();
        (*stats_)[ind].SetPosition(current_position.x + new_x - GetPosition().x,
                                   current_position.y + new_y - GetPosition().y);
    }

    SetPosition(new_x, new_y);
}

void StatsWindow::Draw(sf::RenderWindow* window) {
    window->draw(GetWindow());
    for (size_t ind = first_visible_row_;
         ind < std::min(first_visible_row_ + visible_rows_, GetRowsAmount()); ind++) {
        (*stats_)[ind].Draw(window);
    }
}
