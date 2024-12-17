#include "deck.h"

Deck::Deck() : random_generator_{std::random_device{}()} {
    cards_.resize(kDefaultDeckSize);
    for (size_t value = 0; value < kCardValues; ++value) {
        for (size_t type = 0;
             type < (static_cast<CardValue>(value) == CardValue::Joker ? 2 : kCardTypes); ++type) {
            cards_.push_back(Card(static_cast<CardType>(type), static_cast<CardValue>(value)));
        }
    }
}

void Deck::ReshuffleDeck() { std::shuffle(cards_.begin(), cards_.end(), random_generator_); }

Card Deck::GetTopCard() {
    Card top = cards_.back();
    cards_.pop_back();
    return top;
}
