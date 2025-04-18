#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

enum class GameType : uint8_t { Blackjack, Poker };

// order in this 2 enums is important for Card comparison operator's
enum class CardType : uint8_t { Clubs, Diamonds, Hearts, Spades };
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

enum class BlackjackAction : uint8_t {
    HIT,
    STAND,
    DOUBLE_OR_HIT,
    DOUBLE_OR_STAND,
    SPLIT,
    SPLIT_IF_DOUBLE,
    DUMMY // callback on it will just throw exception
};

const std::array<std::string, 4> type_names = {"hearts", "clubs", "diamonds", "spades"};
const std::array<std::string, 13> value_names = {"two",   "three", "four", "five", "six",
                                                 "seven", "eight", "nine", "ten",  "jack",
                                                 "queen", "king",  "ace"};

constexpr size_t kValueBits = 4;
constexpr size_t kCardTypes = 4;
constexpr size_t kCardValues = 14; // WITH JOKER

constexpr size_t kDefaultDeckSize = 52;

// BLACKJACK STRATEGY TABLE
#define D BlackjackAction::DUMMY
#define H BlackjackAction::HIT
#define S BlackjackAction::STAND
#define DH BlackjackAction::DOUBLE_OR_HIT
#define DS BlackjackAction::DOUBLE_OR_STAND
#define SP BlackjackAction::SPLIT
#define SD BlackjackAction::SPLIT_IF_DOUBLE

// in all tables dealers ace = 11

// hand without ace
// table[our total][dealers total]
constexpr std::array<std::array<BlackjackAction, 12>, 21> kHardTotalsTable{{
    // ACTION                                          Total
    {D, D, D, D, D, D, D, D, D, D, D, D},           // 0
    {D, D, D, D, D, D, D, D, D, D, D, D},           // 1
    {D, D, D, D, D, D, D, D, D, D, D, D},           // 2
    {D, D, D, D, D, D, D, D, D, D, D, D},           // 3
    {D, D, H, H, H, H, H, H, H, H, H, H},           // 4
    {D, D, H, H, H, H, H, H, H, H, H, H},           // 5
    {D, D, H, H, H, H, H, H, H, H, H, H},           // 6
    {D, D, H, H, H, H, H, H, H, H, H, H},           // 7
    {D, D, H, H, H, H, H, H, H, H, H, H},           // 8
    {D, D, H, DH, DH, DH, DH, H, H, H, H, H},       // 9
    {D, D, DH, DH, DH, DH, DH, DH, DH, DH, H, H},   // 10
    {D, D, DH, DH, DH, DH, DH, DH, DH, DH, DH, DH}, // 11
    {D, D, H, H, S, S, S, H, H, H, H, H},           // 12
    {D, D, S, S, S, S, S, H, H, H, H, H},           // 13
    {D, D, S, S, S, S, S, H, H, H, H, H},           // 14
    {D, D, S, S, S, S, S, H, H, H, H, H},           // 15
    {D, D, S, S, S, S, S, H, H, H, H, H},           // 16
    {D, D, S, S, S, S, S, S, S, S, S, S},           // 17
    {D, D, S, S, S, S, S, S, S, S, S, S},           // 18
    {D, D, S, S, S, S, S, S, S, S, S, S},           // 19
    {D, D, S, S, S, S, S, S, S, S, S, S},           // 20
}};

// hand with ace
// table[our second card value][dealers total]
constexpr std::array<std::array<BlackjackAction, 12>, 10> kSoftTotalsTable{{
    // ACTION                                     Total
    {D, D, D, D, D, D, D, D, D, D, D, D},      // 0
    {D, D, D, D, D, D, D, D, D, D, D, D},      // 1
    {D, D, H, H, H, DH, DH, H, H, H, H, H},    // 2
    {D, D, H, H, H, DH, DH, H, H, H, H, H},    // 3
    {D, D, H, H, DH, DH, DH, H, H, H, H, H},   // 4
    {D, D, H, H, DH, DH, DH, H, H, H, H, H},   // 5
    {D, D, H, DH, DH, DH, DH, H, H, H, H, H},  // 6
    {D, D, DS, DS, DS, DS, DS, S, S, H, H, H}, // 7
    {D, D, S, S, S, S, DS, S, S, S, S, S},     // 8
    {D, D, S, S, S, S, S, S, S, S, S, S},      // 9
}};

// hand of pairs
// table[one card from pair value][dealers total]
constexpr std::array<std::array<BlackjackAction, 12>, 12> kPairSplitTable{{
    // ACTION                                          Total
    {D, D, D, D, D, D, D, D, D, D, D, D},           // 0
    {D, D, D, D, D, D, D, D, D, D, D, D},           // 1
    {D, D, SD, SD, SP, SP, SP, SP, S, S, S, S},     // 2
    {D, D, SD, SD, SP, SP, SP, SP, S, S, S, S},     // 3
    {D, D, S, S, S, SD, SD, S, S, S, S, S},         // 4
    {D, D, S, S, S, S, S, S, S, S, S, S},           // 5
    {D, D, SD, SP, SP, SP, SP, S, S, S, S, S},      // 6
    {D, D, SP, SP, SP, SP, SP, SP, S, S, S, S},     // 7
    {D, D, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP}, // 8
    {D, D, SP, SP, SP, SP, SP, S, SP, SP, S, S},    // 9
    {D, D, S, S, S, S, S, S, S, S, S, S},           // 10
    {D, D, SP, SP, SP, SP, SP, SP, SP, SP, SP, SP}  // 11
}};

#undef D
#undef H
#undef S
#undef DH
#undef DS
#undef SP
#undef SD

enum class PokerMove : uint8_t { FOLD = 0, CALL, BET, RAISE, ALL_IN };

struct PokerMoveState {
    PokerMove move;
    std::optional<size_t> money_amount;
};

// Animation constants
constexpr size_t default_delay = 50;
constexpr size_t default_antimation_end_delay = 2000;

// Poker places game object names
const std::array<std::vector<std::string>, 6> kGamblersPlaces = {
    {{"first_place_woman1_1", "first_place_woman3_2", "first_place_woman1_2",
      "first_place_woman3_1", "first_place_man2_1", "first_place_man2_2", "first_place_woman2_1",
      "first_place_man1_1", "first_place_man3_2", "first_place_man1_2", "first_place_man3_1",
      "first_place_woman2_2"},
     {"second_place_woman3_1", "second_place_woman1_1", "second_place_woman2_1",
      "second_place_man1_1", "second_place_man3_1", "second_place_man2_1"},
     {
         "third_place_woman3_1",
         "third_place_woman1_1",
         "third_place_woman2_1",
         "third_place_man1_1",
         "third_place_man2_1",
         "third_place_man3_1",
     },
     {"fourth_place_woman3_1", "fourth_place_woman1_1", "fourth_place_woman2_1",
      "fourth_place_man1_1", "fourth_place_man2_1", "fourth_place_man3_1"},
     {"fivth_place_woman1_1", "fivth_place_woman3_1", "fivth_place_man2_1", "fivth_place_woman2_1",
      "fivth_place_man1_1", "fivth_place_man3_1"},
     {"sixth_place_woman1_1", "sixth_place_woman3_1", "sixth_place_man2_1", "sixth_place_woman2_1",
      "sixth_place_man1_1", "sixth_place_man3_1"}}};
