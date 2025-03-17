#include "gambler.hpp"

#include <utility>

Card BaseGambler::ReturnOneCard() {
    Card last_card = cards_.back();
    cards_.pop_back();
    return last_card;
}

std::vector<Card> BaseGambler::ReturnAllCards() { return std::move(cards_); }

bool BaseGambler::PerformBet(size_t amount) {
    if (amount > money_) {
        return false;
    }

    money_ -= amount;
    return true;
}
