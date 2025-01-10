#pragma once

#include <list>
#include <unordered_map>
#include <utility>

#include "constants.h"
#include "deck.h"
#include "table.h"

struct Hand {
    Hand() : bet(0), player_ind(0), cards_pos(std::make_pair(0, 0)) {}

    Hand(size_t in_bet, size_t in_player_ind, std::pair<size_t, size_t> in_cards_pos)
        : bet(in_bet), player_ind(in_player_ind), cards_pos(in_cards_pos) {}

    size_t bet;
    size_t player_ind;
    std::pair<size_t, size_t> cards_pos; // index of the first and the last card corresponding
                                         // to this hand, [first, last)
};

// Player by zero index is always a dealer
// Also whose_move iterating through hands, not players!!!
class BlackjackTable : public AbstractTable {
    size_t max_players_amount_ = 8;
    struct TableSetting {
        size_t min_bet;
        bool das; // is double after split enabled
    };

public:
    BlackjackTable()
        : AbstractTable(GameType::Blackjack), deck_(true), hands_(), settings_{100, true} {}

    void Dealing() override;

    void GameIteration() override;

    void Clean() override;

    bool IsGameFinished() const override;

private:
    Deck deck_;
    std::list<Hand> hands_;
    std::list<Hand>::iterator hands_iterator_;
    TableSetting settings_;

    friend void HitAction(BlackjackTable* table, Hand&);
    friend void StandAction(BlackjackTable* table, Hand&);
    friend void DoubleOrHitAction(BlackjackTable* table, Hand&);
    friend void DoubleOrStandAction(BlackjackTable* table, Hand&);
    friend void SplitAction(BlackjackTable* table, Hand& hand);
    friend void SplitIfDoubleAction(BlackjackTable* table, Hand& hand);
};

void DummyAction(BlackjackTable* table, Hand&);

void HitAction(BlackjackTable* table, Hand&);

void StandAction(BlackjackTable* table, Hand&);

void DoubleOrHitAction(BlackjackTable* table, Hand&);

void DoubleOrStandAction(BlackjackTable* table, Hand&);

void SplitAction(BlackjackTable* table, Hand& hand);

void SplitIfDoubleAction(BlackjackTable* table, Hand& hand);

const std::unordered_map<BlackjackAction, void (*)(BlackjackTable*, Hand&)> kBlackjackActionsTable{
    {{BlackjackAction::DUMMY, DummyAction},
     {BlackjackAction::HIT, HitAction},
     {BlackjackAction::STAND, StandAction},
     {BlackjackAction::SPLIT, SplitAction},
     {BlackjackAction::SPLIT_IF_DOUBLE, SplitIfDoubleAction},
     {BlackjackAction::DOUBLE_OR_HIT, DoubleOrHitAction},
     {BlackjackAction::DOUBLE_OR_STAND, DoubleOrStandAction}}};
