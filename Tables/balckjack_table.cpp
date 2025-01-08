#include <functional>
#include <memory>
#include <numeric>
#include <stdexcept>

#include "blackjack_table.h"
#include "constants.h"

// cards through 2 to 10 rate as their face value
// J Q K = 10
// Ace = 1 / 11
int RateCard(Card card, int AceValue = 11) {
    if (card.GetValue() == CardValue::Ace) {
        return AceValue;
    }

    int value = static_cast<int>(card.GetValue());
    if (value + 2 <= 10) {
        return value + 2;
    }

    return 10;
}

int GetBestPlayerScore(std::shared_ptr<IGambler> player) {
    static auto take_best_score = [](size_t a, size_t b) noexcept {
        if (a > b && a <= 21) {
            return a;
        }
        return b;
    };

    const std::vector<Card>& player_cards_ = player->ShowCards();
    std::function<int(size_t, size_t)> recursive_calc;

    recursive_calc = [player_cards_, &recursive_calc](size_t ind, size_t cur_score) -> int {
        if (ind >= player_cards_.size()) {
            return cur_score;
        }

        if (player_cards_[ind].GetValue() == CardValue::Ace) {
            return take_best_score(
                recursive_calc(ind + 1, cur_score + RateCard(player_cards_[ind], 1)),
                recursive_calc(ind + 1, cur_score + RateCard(player_cards_[ind], 11)));
        }

        return recursive_calc(ind + 1, cur_score + RateCard(player_cards_[ind]));
    };

    return recursive_calc(0, 0);
}

BlackjackAction
HumbleGambler::BlackjackAction(const std::vector<std::shared_ptr<IGambler>>& players) {
    if (players.size() < 2 || ShowCards().size() < 2) {
        throw std::logic_error("ill formed state passed to BlackjackAction for HumbleGambler");
    }

    Card dealer_open = players[0]->ShowCards()[0];
    const std::vector<Card>& cards = ShowCards();

    if (cards[0].GetValue() == cards[1].GetValue()) {
        return kPairSplitTable[RateCard(cards[0])][RateCard(dealer_open)];
    } else if (cards[0].GetValue() == CardValue::Ace || cards[1].GetValue() == CardValue::Ace) {
        int non_soft_total = RateCard(cards[0], 1) + RateCard(cards[1], 1) - 1;
        return kSoftTotalsTable[non_soft_total][RateCard(dealer_open)];
    }

    return kHardTotalsTable[std::min(20, RateCard(cards[0], 1) + RateCard(cards[1], 1))]
                           [RateCard(dealer_open)];
}

void DummyAction(BlackjackTable* table, size_t player_ind) {
    throw std::logic_error("Something went wrong, hello from DummyAction");
}

void HitAction(BlackjackTable* table, size_t player_ind) {
    const auto player = table->players_[player_ind];
    player->GetCard(table->deck_.GetTopCard());
    if (GetBestPlayerScore(player) >= 21) {
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

    for (size_t ind = 0; ind < players_.size(); ind++) {
        players_[ind]->GetCard(deck_.GetTopCard());
        players_[ind]->GetCard(deck_.GetTopCard());
    }
}

void BlackjackTable::RestartGame() {
    size_t dealer_score = GetBestPlayerScore(players_[0]);
    std::vector<Card> dealer_cards = players_[0]->TakeAllCards();
    deck_.ReturnCards(dealer_cards);

    for (size_t player_ind = 1; player_ind < players_.size(); player_ind++) {
        size_t player_score = GetBestPlayerScore(players_[player_ind]);

        std::vector<Card> cards = players_[player_ind]->TakeAllCards();
        deck_.ReturnCards(cards);

        players_[player_ind]->GetMoney(bets_[player_ind] *
                                       (player_score >= dealer_score && player_score <= 21));
        players_[player_ind]->GetMoney(bets_[player_ind] *
                                       (player_score > dealer_score && player_score <= 21));
    }

    whose_move_ = 0;
}

void BlackjackTable::GameIteration() {
    whose_move_ += whose_move_ == 0;
    if (whose_move_ >= players_.size()) {
        return;
    }

    if (GetBestPlayerScore(players_[whose_move_]) >= 21) {
        players_[whose_move_]->ChangeGameStatus();
        ++whose_move_;
        return;
    }

    BlackjackAction player_decision = players_[whose_move_]->BlackjackAction(players_);
    kBlackjackActionsTable.at(player_decision)(this, whose_move_);
    whose_move_ += !players_[whose_move_]->GetGameStatus();
}

std::shared_ptr<IGambler> BlackjackTable::GetNextPlayer() {
    while (!players_[whose_move_]->GetGameStatus()) {
        ++whose_move_;
        whose_move_ %= players_.size();
    }

    return players_[whose_move_];
}
