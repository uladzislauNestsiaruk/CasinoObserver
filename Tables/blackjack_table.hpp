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

// whose_move iterating through hands, not players!!!
class BlackjackTable : public AbstractTable {
    static constexpr size_t kGamblersPlaces = 6;
    struct TableSetting {
        size_t min_bet;
        bool das; // is double after split enabled
    };

public:
    BlackjackTable(TSQueue<json>& logs, TSQueue<json>& render_queue,
                   AnimationsManager& animations_manager);

    void SetIsActiveGame(bool value) override {}
    bool IsGameFinished() const override;

    void GameIteration() override;
    void Clean() override;

    void ClearRollback() override;
    void RollbackGame() override {}

private:
    void AddDealingPhase(const std::string& phase, size_t delay);
    void AddCardPhase(Card card, size_t card_id, size_t delay);

private:
    std::atomic<bool> is_game_finished_;
    std::list<Hand> hands_;
    std::list<Hand>::iterator hands_iterator_;
    std::vector<Card> dealer_cards_;
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
