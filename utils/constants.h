#ifndef CASINOOBSERVER_BRAINMANAGER_CONSTANTS_H
#define CASINOOBSERVER_BRAINMANAGER_CONSTANTS_H

#include <cstddef>
#include <cstdint>

enum class GameType : uint8_t { Blackjack, Poker };

enum class CardType : uint8_t { Hearts, Clubs, Diamonds, Spades };

enum class CardValue : uint8_t {
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

constexpr size_t kDefaultDeckSize = 52;

#endif
