#include "gambler.hpp"

std::vector<Card> BaseGambler::TakeAllCards() { return std::move(cards_); }
