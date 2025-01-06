#pragma once

#include <memory>
#include <unordered_map>

#include "constants.h"
#include "table.h"

// Player by zero index is always a dealer
class BlackjackTable : public AbstractTable {
public:
    BlackjackTable() : AbstractTable(), bets_(8, 0) {}

    void Dealing() override;

    void GameIteration() override;

private:
    // returns first player in the order that still in game
    std::shared_ptr<IGambler> GetNextPlayer();

private:
    Deck deck_;
    std::vector<size_t> bets_;

    friend void HitAction(BlackjackTable* table, size_t player_ind);
    friend void StandAction(BlackjackTable* table, size_t player_ind);
    friend void DoubleOrHitAction(BlackjackTable* table, size_t player_ind);
    friend void DoubleOrStandAction(BlackjackTable* table, size_t player_ind);
};

void DummyAction(BlackjackTable* table, size_t player_ind);

void HitAction(BlackjackTable* table, size_t player_ind);

void StandAction(BlackjackTable* table, size_t player_ind);

void DoubleOrHitAction(BlackjackTable* table, size_t player_ind);

void DoubleOrStandAction(BlackjackTable* table, size_t player_ind);

const std::unordered_map<BlackjackAction, void (*)(BlackjackTable*, size_t player_ind)>
    kBlackjackActionsTable{{{BlackjackAction::DUMMY, DummyAction},
                            {BlackjackAction::HIT, HitAction},
                            {BlackjackAction::STAND, StandAction},
                            {BlackjackAction::DOUBLE_OR_HIT, DoubleOrHitAction},
                            {BlackjackAction::DOUBLE_OR_STAND, DoubleOrStandAction}}};
