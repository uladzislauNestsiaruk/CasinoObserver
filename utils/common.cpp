#include "common.hpp"

#include <random>

uint64_t get_random_number(uint64_t min, uint64_t max) {
    static std::random_device rd;
    static std::mt19937_64 gen(rd());
    std::uniform_int_distribution<uint64_t> dist(min, max);

    return dist(gen);
}

std::string ExtractPersonPlace(const std::string& player_tag) {
    size_t pos = 0;
    for (size_t i = 0; i < 2; i++) {
        pos = player_tag.find('_', pos) + 1;
    }

    return player_tag.substr(0, pos - 1);
}

std::vector<std::string> GetTableCardObjectsTags(GameType game_type) {
    std::vector<std::string> tags;
    if (game_type == GameType::Poker) {
        for (const std::string& card_id : {"first", "second", "third", "fourth", "fivth"}) {
            tags.push_back(card_id + "_central_card");
        }
    } else {
        for (size_t player_id = 1; player_id <= 6; ++player_id) {
            for (size_t card_num = 1; card_num <= 2; ++card_num) {
                tags.push_back("player_" + std::to_string(player_id) + "_" +
                               std::to_string(card_num));
            }
        }

        tags.push_back("dealer_1");
    }

    return tags;
}