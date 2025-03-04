#pragma once

#include "table.hpp"
#include <common.hpp>
#include <constants.hpp>
#include <deck.hpp>
#include <json.hpp>
#include <thread_safe_queue.hpp>

#include <string_view>
#include <vector>
#include <queue>

using json = nlohmann::json;

class PokerTable : public AbstractTable {
    static constexpr size_t kGamblersPlaces = 6;
public:
    PokerTable(TSQueue<json>& logs, TSQueue<json>& render_queue);

    void GameIteration() override;

    bool IsGameFinished() const override;

    void Clean() override;

private:

    void Dealing() override;

    void ApplyBets();

    void MakeBet(size_t amount, size_t player_ind);

    void DistributionPhase(std::string_view phase);

    void BettingPhase();

    void SelectWinners();

    void RollbackGame();

private:
    size_t active_players_ = 0; // Number of players which can make bets
    size_t all_in_players_ = 0; // Number of players which are all in
    bool show_all_cards_ = 0;   // true if all players are all in or there is only one active player
    std::vector<bool> is_all_in_; // true if player with this index is all in

    std::atomic<bool> is_active_game = false;

    struct BetState {
        size_t amount = 0;
        size_t num_raises = 0;
    };

    std::vector<Card> table_cards_;

    std::vector<BetState> bets_;
    size_t current_bet_ = 0;
    size_t bank_ = 0;
    size_t min_bet_;
    size_t min_raise_;
    size_t small_blind_;
    size_t big_blind_;

    TSQueue<json>& logs_;
    std::queue<json> rollback_logs_;
};
