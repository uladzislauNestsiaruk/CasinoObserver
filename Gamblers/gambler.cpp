#include "gambler.h"

std::vector<Card> BaseGambler::TakeAllCards() {
    std::vector<Card> return_value(cards_);
    cards_.clear();
    return return_value;
}
