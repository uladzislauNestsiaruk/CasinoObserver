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

size_t BaseGambler::GetBalanceLevel() const {
    if (money_ == 0) {
        return 0;
    }

    return std::min(6ul, money_ / 300 + 1);
}