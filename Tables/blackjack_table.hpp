#pragma once

#include <list>
#include <utility>

#include "table.hpp"
#include <constants.hpp>
#include <deck.hpp>

using json = nlohmann::json;

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
    size_t max_players_amount_ = 6;
    struct TableSetting {
        size_t min_bet;
        bool das; // is double after split enabled
    };

public:
    BlackjackTable(TSQueue<json>& logs, TSQueue<json>& render_queue)
        : AbstractTable(GameType::Blackjack, render_queue), deck_(true), is_game_finished_(true),
          hands_(), settings_{100, true}, logs_(logs), render_queue_(render_queue) {
        AddPlayer(
            std::make_shared<HumbleGambler>(0, 0, GameType::Blackjack, false, 1000, "dummy_tag"));
        AddPlayer(
            std::make_shared<HumbleGambler>(0, 1, GameType::Blackjack, false, 2000, "dummy_tag"));
    }

    void GameIteration() override;

    void Clean() override;

    bool IsGameFinished() const override;

private:
    Deck deck_;
    std::atomic<bool> is_game_finished_;
    std::list<Hand> hands_;
    std::list<Hand>::iterator hands_iterator_;
    TableSetting settings_;
    TSQueue<json>& logs_;
    TSQueue<json>& render_queue_;

    void Dealing() override;

    friend void HitAction(BlackjackTable* table, Hand&, json&);
    friend void StandAction(BlackjackTable* table, Hand&, json&);
    friend void DoubleOrHitAction(BlackjackTable* table, Hand&, json&);
    friend void DoubleOrStandAction(BlackjackTable* table, Hand&, json&);
    friend void SplitAction(BlackjackTable* table, Hand& hand, json&);
    friend void SplitIfDoubleAction(BlackjackTable* table, Hand& hand, json&);
};

void DummyAction(BlackjackTable* table, Hand&, json&);

void HitAction(BlackjackTable* table, Hand&, json&);

void StandAction(BlackjackTable* table, Hand&, json&);

void DoubleOrHitAction(BlackjackTable* table, Hand&, json&);

void DoubleOrStandAction(BlackjackTable* table, Hand&, json&);

void SplitAction(BlackjackTable* table, Hand& hand, json&);

void SplitIfDoubleAction(BlackjackTable* table, Hand& hand, json&);

const std::unordered_map<BlackjackAction, void (*)(BlackjackTable*, Hand&, json&)>
    kBlackjackActionsTable{{{BlackjackAction::DUMMY, DummyAction},
                            {BlackjackAction::HIT, HitAction},
                            {BlackjackAction::STAND, StandAction},
                            {BlackjackAction::SPLIT, SplitAction},
                            {BlackjackAction::SPLIT_IF_DOUBLE, SplitIfDoubleAction},
                            {BlackjackAction::DOUBLE_OR_HIT, DoubleOrHitAction},
                            {BlackjackAction::DOUBLE_OR_STAND, DoubleOrStandAction}}};
