#include "common.hpp"

#include <random>

uint64_t get_random_number(uint64_t min, uint64_t max) {
    static std::random_device rd;
    static std::mt19937_64 gen(rd());
    std::uniform_int_distribution<uint64_t> dist(min, max);

    return dist(gen);
}