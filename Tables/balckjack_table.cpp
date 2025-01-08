#include <memory>
#include <stdexcept>

#include "blackjack_table.h"
#include "constants.h"

int RateCard(Card card, int AceValue = 11) {
    int value = static_cast<int>(card.GetValue());
    if (value <= 10) {
        return value;
    }

    if (value == kCardValues - 1) {
        return AceValue;
    }

    return 10;
}

int GetPlayerScore(std::shared_ptr<IGambler> player) {
    return RateCard(player->ShowCards()[0], 1) + RateCard(player->ShowCards()[1], 1);
}

BlackjackAction HumbleGambler::BlackjackAction(const std::vector<Card>& state) {
    if (state.size() < 3) {
        throw std::logic_error("ill formed state passed to BlackjackAction for HumbleGambler");
    }

    Card dealer_open = state[0];
    const std::vector<Card>& cards = ShowCards();

    if (cards[0].GetValue() == cards[1].GetValue()) {
        return kPairSplitTable[RateCard(cards[0])][RateCard(dealer_open)];
    } else if (cards[0].GetValue() == CardValue::Ace || cards[1].GetValue() == CardValue::Ace) {
        int non_soft_total = RateCard(cards[0], 1) + RateCard(cards[1], 1) - 1;
        return kSoftTotalsTable[non_soft_total][RateCard(dealer_open)];
    }

    return kHardTotalsTable[RateCard(cards[0], 1) + RateCard(cards[1], 1)][RateCard(dealer_open)];
}

void DummyAction(BlackjackTable* table, size_t player_ind) {
    throw std::logic_error("Something went wrong, hello from DummyAction");
}

void HitAction(BlackjackTable* table, size_t player_ind) {
    const auto player = table->players_[player_ind];
    player->GetCard(table->deck_.GetTopCard());
    if (GetPlayerScore(player) >= 21) {
        player->ChangeGameStatus();
    }
}

void StandAction(BlackjackTable* table, size_t player_ind) {
    table->players_[player_ind]->ChangeGameStatus();
}

void DoubleOrHitAction(BlackjackTable* table, size_t player_ind) {
    auto player = table->players_[player_ind];
    if (player->PerformBet(table->bets_[player_ind])) {
        player->GetCard(table->deck_.GetTopCard());
        player->ChangeGameStatus();
        return;
    }

    player->GetCard(table->deck_.GetTopCard());
}

void DoubleOrStandAction(BlackjackTable* table, size_t player_ind) {
    auto player = table->players_[player_ind];
    if (player->PerformBet(table->bets_[player_ind])) {
        player->GetCard(table->deck_.GetTopCard());
    }

    player->ChangeGameStatus();
}

void BlackjackTable::Dealing() {
    // No players at the table
    if (players_.size() <= 1) {
        return;
    }

    deck_.ReshuffleDeck();
    table_state_.clear();
    table_state_.resize(players_.size() * 2 - 1);

    for (size_t ind = 0; ind < players_.size(); ind++) {
        players_[ind]->GetCard(deck_.GetTopCard());
        players_[ind]->GetCard(deck_.GetTopCard());
        table_state_[2 * ind - 1] = players_[ind]->ShowCards()[0];
        table_state_[2 * ind] = players_[ind]->ShowCards()[1];
    }
}

void BlackjackTable::FinalStage() {
    size_t dealer_score = GetPlayerScore(players_[0]);

    for (size_t player_ind = 1; player_ind < players_.size(); player_ind++) {
        size_t player_score = GetPlayerScore(players_[player_ind]);
        if (player_score > 21 || player_score < dealer_score) {
            continue;
        }

        players_[player_ind]->GetMoney(bets_[player_ind]);
        players_[player_ind]->GetMoney(bets_[player_ind] * (player_score > dealer_score));
    }
}

void BlackjackTable::GameIteration() {
    if (whose_move_ < players_.size()) {
        BlackjackAction player_decision = players_[whose_move_]->BlackjackAction(table_state_);
        kBlackjackActionsTable.at(player_decision)(this, whose_move_);
        whose_move_ += !players_[whose_move_]->GetGameStatus();
        return;
    }

    FinalStage();
    whose_move_ = 0;
}

std::shared_ptr<IGambler> BlackjackTable::GetNextPlayer() {
    while (!players_[whose_move_]->GetGameStatus()) {
        ++whose_move_;
        whose_move_ %= players_.size();
    }

    return players_[whose_move_];
}
