#include "table.h"

void AbstractTable::AddPlayer(std::shared_ptr<IGambler> player) {
    players_.emplace_back(player);
    uint8_t place = player->GetTableSeatId();
    render_queue_.push({{"event_type", "change_phase"},
                        {"new_phase", "afk"},
                        {"tag", player->GetPersonTag()},
                        {"delay", 0}});
}

void AbstractTable::RemovePlayer(const IGambler& player) {
    for (size_t ind = 0; ind < players_.size(); ind++) {
        if (&(*players_[ind]) == &player) {
            players_.erase(players_.begin() + ind);
            break;
        }
    }
}

void AbstractTable::Update(sf::Time delta) {
    elapsed_ += delta;
    if (elapsed_ >= time_per_action_) {
        elapsed_ -= time_per_action_;
        GameIteration();
    }
}