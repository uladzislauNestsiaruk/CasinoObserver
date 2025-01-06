#pragma once

#include "table.h"

class PokerTable : public AbstractTable {
public:
    PokerTable() : AbstractTable(), min_bet_(100), min_raise_(50), small_blind_(50), big_blind_(min_bet_) {}

    void Dealing() override;

    void GameIteration() override;

private:
    void ApplyBets();

    void MakeBet(size_t amount, size_t player_ind);

    void DistributionPhase(std::string_view phase);

    void BettingPhase();

private:
    struct BetState {
        size_t amount = 0;
        size_t num_raises = 0;
    };

    Deck deck_;
    std::vector<Card> table_cards_;

    std::vector<BetState> bets_;
    size_t current_bet_ = 0;
    size_t bank_ = 0;
    size_t min_bet_;
    size_t min_raise_;
    size_t small_blind_;
    size_t big_blind_;
};
