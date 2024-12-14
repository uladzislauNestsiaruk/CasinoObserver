#pragma once

#include "card.h"

#include <random>
#include <vector>

class Deck {
public:
    Deck();

    void ReshuffleDeck();

    Card GetTopCard();

private:
    std::vector<Card> cards_;
    std::mt19937 random_generator_;
};