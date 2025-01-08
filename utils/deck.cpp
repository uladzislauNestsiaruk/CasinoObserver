#include "deck.h"
#include <iterator>

Deck::Deck(bool without_jokers) : random_generator_{std::random_device{}()} {
    cards_.reserve(kDefaultDeckSize);
    for (size_t value = 0; value < kCardValues - static_cast<size_t>(without_jokers); ++value) {
        for (size_t type = 0;
             type < (static_cast<CardValue>(value) == CardValue::Joker ? 2 : kCardTypes); ++type) {
            cards_.emplace_back(Card(static_cast<CardType>(type), static_cast<CardValue>(value)));
        }
    }
}

void Deck::RemoveCards(const std::vector<Card>& cards) {
    cards_.erase(std::remove_if(cards_.begin(), cards_.end(),
                                [&](const Card& card) {
                                    return std::find_if(cards.begin(), cards.end(),
                                                        [&](const Card& card_rm) {
                                                            return card == card_rm;
                                                        }) != cards.end();
                                }),
                 cards_.end());
}

void Deck::ReshuffleDeck() { std::shuffle(cards_.begin(), cards_.end(), random_generator_); }

Card Deck::GetTopCard() {
    Card top = cards_.back();
    cards_.pop_back();
    return top;
}

void Deck::ReturnCards(std::vector<Card>& cards) {
    cards_.insert(cards_.end(), std::make_move_iterator(cards.begin()),
                  std::make_move_iterator(cards.end()));
}
