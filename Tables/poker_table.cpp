#include "poker_table.h"

namespace {
constexpr uint32_t num_iterations = 10000;

// Rate introduced by pair <id of combination increasing by priority, card values which
// characterized this combination>
std::pair<size_t, std::vector<CardValue>> RateHand(const std::vector<Card>& table_cards,
                                                   const std::vector<Card>& hand) {
    std::vector<Card> all_cards(table_cards);
    all_cards.insert(all_cards.end(), hand.begin(), hand.end());

    std::unordered_map<CardType, std::vector<CardValue>> type_map;
    std::unordered_map<CardValue, size_t> value_map;
    for (const auto& card : all_cards) {
        type_map[card.GetType()].push_back(card.GetValue());
        ++value_map[card.GetValue()];
    }

    CardValue greater_card;
    std::vector<CardValue> pairs;
    std::vector<CardValue> three_of_a_kind;
    std::vector<CardValue> four_of_a_kind;
    CardValue straight;
    std::vector<CardValue> flash;
    std::vector<CardValue> straight_flash;
    // Check flash and straight flash
    for (auto& cards : type_map) {
        if (cards.second.size() < 5) {
            continue;
        }

        std::sort(cards.second.begin(), cards.second.end());
        std::reverse(cards.second.begin(), cards.second.end());
        std::vector<CardValue> five_highest(cards.second.begin(), cards.second.begin() + 5);
        flash = std::max(flash, five_highest);
        for (size_t i = 0; i + 4 < cards.second.size(); ++i) {
            if (static_cast<uint8_t>(cards.second[i]) - 4 ==
                static_cast<uint8_t>(cards.second[i + 4])) {
                std::vector<CardValue> temp(cards.second.begin() + i, cards.second.begin() + i + 5);
                straight_flash = std::max(straight_flash, temp);
                break;
            }
        }
    }
    // Finding all pairs, sets, straights and fours of a kind
    for (auto& value : value_map) {
        if (value.second == 4) {
            four_of_a_kind.push_back(value.first);
            three_of_a_kind.push_back(value.first);
            pairs.push_back(value.first);
            pairs.push_back(value.first);
        } else if (value.second == 3) {
            three_of_a_kind.push_back(value.first);
            pairs.push_back(value.first);
        } else if (value.second == 2) {
            pairs.push_back(value.first);
        } else if (value.second == 1) {
            greater_card = std::max(greater_card, value.first);
        }

        bool no_straight = false;
        for (size_t i = 1; i < 5; ++i) {
            no_straight |=
                !value_map.contains(static_cast<CardValue>(static_cast<uint8_t>(value.first) - i));
        }

        if (!no_straight) {
            straight = std::max(straight, value.first);
        }
    }

    if (!straight_flash.empty()) {
        return {(straight_flash[0] == CardValue::Ace ? 10 : 9), {straight_flash[0]}};
    } else if (!four_of_a_kind.empty()) {
        std::sort(four_of_a_kind.begin(), four_of_a_kind.end());
        return {8, {four_of_a_kind.back()}};
    } else if (!three_of_a_kind.empty() && !pairs.empty()) {
        std::sort(three_of_a_kind.begin(), three_of_a_kind.end());
        std::sort(pairs.begin(), pairs.end());
        if (pairs.size() == 1 && three_of_a_kind.size() == 1) {
            return {4, {three_of_a_kind.back()}};
        }

        return {
            7,
            {three_of_a_kind.back(),
             (pairs.back() != three_of_a_kind.back() ? pairs.back() : pairs[pairs.size() - 2])}};
    } else if (!flash.empty()) {
        return {6, flash};
    } else if (static_cast<uint8_t>(straight) != 0) {
        return {5, {straight}};
    } else if (!three_of_a_kind.empty()) {
        std::sort(three_of_a_kind.begin(), three_of_a_kind.end());
        return {4, {three_of_a_kind.back()}};
    } else if (!pairs.empty()) {
        std::sort(pairs.begin(), pairs.end());
        if (pairs.size() > 1) {
            return {3, {pairs.back(), pairs[pairs.size() - 2]}};
        }

        return {2, {pairs.back()}};
    } else {
        return {1, {greater_card}};
    }
}

double WinProbability(size_t num_opponents, const std::vector<Card>& table_cards,
                      const std::vector<Card>& hand) {
    std::vector<Card> all_cards(table_cards);
    all_cards.insert(all_cards.end(), hand.begin(), hand.end());

    Deck exp_deck;
    exp_deck.RemoveCards(all_cards);
    uint16_t wins = 0;
    uint16_t ties = 0;

    std::vector<Card> exp_table_cards(table_cards);
    for (size_t i = 0; i < num_iterations; ++i) {
        exp_deck.ReshuffleDeck();
        for (size_t j = 0; j < 5 - table_cards.size(); ++j) {
            exp_table_cards.push_back(exp_deck.GetTopCard());
        }

        bool at_least_tie = true;
        bool win = true;

        std::pair<size_t, std::vector<CardValue>> hand_rate = RateHand(exp_table_cards, hand);
        for (size_t j = 0; j < num_opponents; ++j) {
            std::pair<size_t, std::vector<CardValue>> opponent_rate =
                RateHand(exp_table_cards, {exp_deck.GetTopCard(), exp_deck.GetTopCard()});
            if (opponent_rate == hand_rate) {
                win = false;
            } else if (opponent_rate > hand_rate) {
                win = at_least_tie = false;
                break;
            }
        }

        if (win) {
            wins += win;
        } else if (at_least_tie) {
            ties += at_least_tie;
        }
    }

    return static_cast<double>(wins + 0.5 * ties) / num_iterations;
}
} // namespace

void PokerTable::MakeBet(size_t amount, size_t player_ind) {
    if (players_[player_ind]->PerformBet(amount)) {
        bank_ += amount;
        return;
    }

    throw std::runtime_error("[ERROR]: Bet is more than player bank");
}

void PokerTable::ApplyBets() {
    for (size_t i = 0; i < players_.size(); ++i) {
        if (players_[i]->GetGameStatus()) {
            MakeBet(bets_[i].amount, i);
        }
    }
}

void PokerTable::Dealing() {
    // Not enough players
    if (players_.size() < 2) {
        return;
    }

    for (auto player : players_) {
        if (!player->GetGameStatus()) {
            player->ChangeGameStatus();
        }
    }

    current_bet_ = 0;
    bets_.assign(players_.size(), {0, 0});
    deck_.ReshuffleDeck();
    for (size_t i = 0; i < players_.size(); ++i) {
        if (players_[i]->GetBalance() < big_blind_) {
            players_[i]->ChangeGameStatus();
            continue;
        }
        bets_[i] = {big_blind_, 0};
    }

    ApplyBets();
}

PokerMoveState HumbleGambler::PokerAction(size_t num_opponents,
                                          const std::vector<Card>& table_cards,
                                          const std::vector<Card>& hand, size_t current_bet,
                                          size_t min_bet, size_t min_raise, size_t num_raises) {
    // First Move (Blinds)
    if (table_cards.size() + hand.size() == 0) {
        return {.move = PokerMove::CALL, .money_amount = current_bet};
    }

    double probability = WinProbability(num_opponents, table_cards, hand);

    double raise = 0.0;
    if (probability > 0.85) {
        raise = (current_bet == 0 ? min_bet * 2.0 : current_bet * 1.5);
    } else if (probability > 0.7) {
        raise = (current_bet == 0 ? min_bet * 1.5 : current_bet * 1.0);
    } else if (probability > 0.55) {
        raise = (current_bet == 0 ? min_bet * 1.25 : current_bet * 0.5);
    } else if (probability > 0.4) {
        raise = (current_bet == 0 ? min_bet * 1.0 : 0.0);
    } else if ((probability < 0.3 && table_cards.size() == 5) ||
               (probability < 0.2 && table_cards.size() == 4) ||
               (probability < 0.1 && table_cards.size() == 3) ||
               (probability < 0.05 && table_cards.empty())) {
        ChangeGameStatus();
        return {.move = PokerMove::FOLD};
    }

    if (num_raises == 2) {
        raise = 0.0;
    }

    size_t total_bet = current_bet + (raise != 0 && raise < min_raise ? min_raise : raise);

    if (total_bet > 0.95 * GetBalance()) {
        ChangeGameStatus();
        return {.move = PokerMove::ALL_IN, .money_amount = GetBalance()};
    }

    if (total_bet > current_bet) {
        return {.move = (current_bet == 0 ? PokerMove::BET : PokerMove::RAISE),
                .money_amount = total_bet};
    }

    return {.move = PokerMove::CALL, .money_amount = current_bet};
}

void PokerTable::DistributionPhase(std::string_view phase) {
    if (phase == "preflop") {
        for (auto player : players_) {
            player->GetCard(deck_.GetTopCard());
            player->GetCard(deck_.GetTopCard());
        }
    } else if (phase == "flop") {
        for (size_t i = 0; i < 3; ++i) {
            table_cards_.push_back(deck_.GetTopCard());
        }
    } else {
        table_cards_.push_back(deck_.GetTopCard());
    }
}

void PokerTable::BettingPhase() {
    for (size_t ind = 0; bets_[ind].amount != current_bet_ || current_bet_ == 0;
         ind = (ind + 1) % players_.size()) {
        if (players_[ind]->GetGameStatus()) {
            PokerMoveState move_state = players_[ind]->PokerAction(
                players_.size(), table_cards_, players_[ind]->ShowCards(), current_bet_, min_bet_,
                min_raise_, bets_[ind].num_raises);
            switch (static_cast<uint8_t>(move_state.move)) {
            case static_cast<uint8_t>(PokerMove::ALL_IN):
                current_bet_ = std::min(current_bet_, move_state.money_amount.value());
                break;
            case static_cast<uint8_t>(PokerMove::BET):
                current_bet_ = move_state.money_amount.value();
                break;
            case static_cast<uint8_t>(PokerMove::RAISE):
                current_bet_ = move_state.money_amount.value();
                ++bets_[ind].num_raises;
                break;
            default:
                break;
            }

            if (move_state.move != PokerMove::FOLD) {
                bets_[ind].amount = move_state.money_amount.value();
            }
        }
    }
}

void PokerTable::GameIteration() {
    // Not enough players
    if (players_.size() < 2) {
        return;
    }

    for (const std::string& phase : {"preflop", "flop", "turn", "river"}) {
        current_bet_ = 0;
        bets_.assign(players_.size(), {0, 0});
        DistributionPhase(phase);
        BettingPhase();
        ApplyBets();
    }
}
