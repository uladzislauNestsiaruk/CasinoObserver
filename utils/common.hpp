#pragma once

#include <constants.hpp>

#include <cstdint>
#include <string>

uint64_t get_random_number(uint64_t min, uint64_t max);

std::string ExtractPersonPlace(const std::string& player_tag);

std::vector<std::string> GetTableCardObjectsTags(GameType game_type);