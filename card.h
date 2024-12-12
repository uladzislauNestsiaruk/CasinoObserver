// "Copyright [2024] Netsiaruk Uladzislau"
#ifndef CASINOOBSERVER_CARD_H_
#define CASINOOBSERVER_CARD_H_

#include <cstddef>
#include <memory>

enum CardType { Hearts, Clubs, Diamonds, Spades };

enum CardValue {
    Two,
    Three,
    Four,
    Five,
    Six,
    Seven,
    Eight,
    Nine,
    Ten,
    Valet,
    Queen,
    King,
    Ace,
    Joker
};

constexpr size_t kValueBits = 4;
constexpr size_t kCardTypes = 4;
constexpr size_t kCardValues = 14; // WITH JOKER

class Card {

  public:
    Card() : value_{0} {}

    Card(CardType type, CardValue value) {
        value_ =
            (static_cast<int>(type) << kValueBits) | static_cast<int>(value);
    }

    CardType GetType() { return CardType(value_ >> kValueBits); }

    CardValue GetValue() { return CardValue(value_ & ((1 << kValueBits) - 1)); }

  private:
    char value_;
};

#endif // CASINOOBSERVER_CARD_H_
