#include "gambler.h"

std::vector<Card> BaseGambler::TakeAllCards() { return std::move(cards_); }
