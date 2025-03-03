#include "table.hpp"

void AbstractTable::AddPlayer(std::shared_ptr<IGambler> player) {
    players_.emplace_back(player);
    occupied_places_[player->GetTableSeatId()] = true;
    render_queue_.push({{"event", {{"type", "change_phase"}}},
                        {"new_phase", "afk"},
                        {"tag", player->GetPersonTag()},
                        {"delay", 0}});
}

void AbstractTable::RemovePlayer(size_t ind) {
    render_queue_.push({{"event", {{"type", "change_phase"}}},
                        {"new_phase", "empty"},
                        {"tag", players_[ind]->GetPersonTag()},
                        {"delay", 0}});
    occupied_places_[players_[ind]->GetTableSeatId()] = false;
    players_.erase(players_.begin() + ind);
}

void AbstractTable::Update(sf::Time delta) {
    elapsed_ += delta;
    if (elapsed_ >= time_per_action_) {
        elapsed_ -= time_per_action_;
        GameIteration();
    }
}
