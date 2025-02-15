#pragma once

#include "card.hpp"

#include <random>
#include <vector>

class Deck {
public:
    Deck(bool without_jokers = false);

    void RemoveCards(const std::vector<Card>& cards);

    void ReshuffleDeck();

    Card GetTopCard();

    void ReturnCards(std::vector<Card>& cards);
    void ReturnCards(std::vector<Card>&& cards);

    bool operator==(Deck& other) {
        if (cards_.size() != other.cards_.size()) {
            return false;
        }

        std::sort(cards_.begin(), cards_.end());
        std::sort(other.cards_.begin(), other.cards_.end());
        for (size_t i = 0; i < cards_.size(); ++i) {
            if (cards_[i] != other.cards_[i]) {
                return false;
            }
        }

        return true;
    }

private:
    std::vector<Card> cards_;
    std::mt19937 random_generator_;
};
