#pragma once

#include "card.h"

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

private:
    std::vector<Card> cards_;
    std::mt19937 random_generator_;
};
