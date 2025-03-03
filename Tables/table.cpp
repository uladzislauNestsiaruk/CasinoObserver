#include "table.hpp"
#include "common.hpp"
#include "constants.hpp"

void AbstractTable::AddPlayer(std::shared_ptr<IGambler> player) {
    players_.emplace_back(player);
    occupied_places_[player->GetTableSeatId()] = true;
    render_queue_.push({{"event", {{"type", "change_phase"}}},
                        {"new_phase", "afk"},
                        {"tag", player->GetPersonTag()},
                        {"delay", 0}});
    // render_queue_.push({{"event_type", "add_handler"},
    //                     {"new_phase", "afk"},
    //                     {"tag", player->GetPersonTag()}});
}

void AbstractTable::RemovePlayer(size_t ind) {
    render_queue_.push({{"event", {{"type", "change_phase"}}},
                        {"new_phase", "empty"},
                        {"tag", players_[ind]->GetPersonTag()},
                        {"delay", 0}});
    occupied_places_[players_[ind]->GetTableSeatId()] = false;
    deck_.ReturnCards(players_[ind]->TakeAllCards());
    players_.erase(players_.begin() + ind);
}

void AbstractTable::Update(sf::Time delta) {
    elapsed_ += delta;
    if (elapsed_ >= time_per_action_) {
        elapsed_ -= time_per_action_;
        GameIteration();
    }
}

void AbstractTable::GenPlayers(uint8_t num_players, uint8_t max_places, GameType type,
                               uint16_t left_money_bound, uint16_t right_money_bound) {
    for (size_t i = 0; i < num_players; ++i) {
        uint8_t place;
        do {
            place = get_random_number(0, max_places);
        } while (occupied_places_[place]);

        std::string person_tag =
            kGamblersPlaces[place][get_random_number(0, kGamblersPlaces[place].size() - 1)];
        AddPlayer(std::make_shared<HumbleGambler>(
            false, place, type, 0, get_random_number(left_money_bound, right_money_bound),
            person_tag));
    }
}

void AbstractTable::RemovePlayer(const std::string& person_tag) {
    for (size_t i = 0; i < players_.size(); ++i) {
        if (players_[i]->GetPersonTag() == person_tag) {
            RemovePlayer(i);
            return;
        }
    }
}
