// "Copyright [2024] Netsiaruk Uladzislau"
#ifndef CASINOOBSERVER_CARD_H_
#define CASINOOBSERVER_CARD_H_

#include "utils/constants.h"

class Card {
public:
    Card() : value_{0} {}

    Card(CardType type, CardValue value) {
        value_ =
            (static_cast<uint8_t>(type) << kValueBits) | static_cast<uint8_t>(value);
    }

    CardType GetType() { return CardType(value_ >> kValueBits); }

    CardValue GetValue() { return CardValue(value_ & ((1 << kValueBits) - 1)); }

private:
    uint8_t value_;
};

#endif // CASINOOBSERVER_CARD_H_
