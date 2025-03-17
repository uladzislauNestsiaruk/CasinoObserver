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