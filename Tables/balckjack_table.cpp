#include <functional>
#include <memory>
#include <stdexcept>
#include <utility>

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

int GetBestPlayerScore(const std::vector<std::shared_ptr<IGambler>>& players, const Hand& hand) {
    static auto take_best_score = [](size_t a, size_t b) noexcept {
        if (a > b && a <= 21) {
            return a;
        }
        return b;
    };

    std::function<int(size_t, size_t)> recursive_calc;
    const std::vector<Card>& player_cards = players[hand.player_ind]->ShowCards();

    recursive_calc = [player_cards, &hand, &recursive_calc](size_t ind, size_t cur_score) -> int {
        if (ind >= hand.cards_pos.second) {
            return cur_score;
        }

        if (player_cards[ind].GetValue() == CardValue::Ace) {
            return take_best_score(
                recursive_calc(ind + 1, cur_score + RateCard(player_cards[ind], 1)),
                recursive_calc(ind + 1, cur_score + RateCard(player_cards[ind], 11)));
        }

        return recursive_calc(ind + 1, cur_score + RateCard(player_cards[ind]));
    };

    return recursive_calc(hand.cards_pos.first, 0);
}

BlackjackAction
HumbleGambler::BlackjackAction(const std::vector<std::shared_ptr<IGambler>>& players,
                               const Hand& hand) {
    const std::vector<Card>& cards = ShowCards();
    if (players.size() < 2 || hand.cards_pos.second - hand.cards_pos.first < 2) {
        throw std::logic_error("ill formed state passed to BlackjackAction for HumbleGambler");
    }

    Card dealer_open = players[0]->ShowCards()[0];

    if (cards.size() == 2 && RateCard(cards[0]) == RateCard(cards[1])) {
        return kPairSplitTable[RateCard(cards[0])][RateCard(dealer_open)];
    } else if (cards.size() == 2 &&
               (cards[0].GetValue() == CardValue::Ace || cards[1].GetValue() == CardValue::Ace)) {
        int non_soft_total = RateCard(cards[0], 1) + RateCard(cards[1], 1) - 1;
        return kSoftTotalsTable[non_soft_total][RateCard(dealer_open)];
    }

    return kHardTotalsTable[GetBestPlayerScore(players, hand)][RateCard(dealer_open)];
}

void DummyAction(BlackjackTable* table, Hand& hand) {
    throw std::logic_error("Something went wrong, hello from DummyAction");
}

void HitAction(BlackjackTable* table, Hand& hand) {
    const auto player = table->players_[hand.player_ind];
    player->GetCard(table->deck_.GetTopCard());
    ++hand.cards_pos.second;
}

void StandAction(BlackjackTable* table, Hand& hand) {
    table->players_[hand.player_ind]->ChangeGameStatus();
}

void DoubleOrHitAction(BlackjackTable* table, Hand& hand) {
    auto player = table->players_[hand.player_ind];
    if (player->PerformBet(hand.bet)) {
        hand.bet <<= 1;
        player->ChangeGameStatus();
    }

    player->GetCard(table->deck_.GetTopCard());
    ++hand.cards_pos.second;
}

void DoubleOrStandAction(BlackjackTable* table, Hand& hand) {
    auto player = table->players_[hand.player_ind];
    if (player->PerformBet(hand.bet)) {
        hand.bet <<= 1;
        player->GetCard(table->deck_.GetTopCard());
        ++hand.cards_pos.second;
    }

    player->ChangeGameStatus();
}

void SplitAction(BlackjackTable* table, Hand& hand) {
    if (!table->players_[hand.player_ind]->PerformBet(hand.bet)) {
        return;
    }

    // strange immitation of doubling
    Card last_card = table->players_[hand.player_ind]->ReturnOneCard();
    table->players_[hand.player_ind]->GetCard(table->deck_.GetTopCard());
    table->players_[hand.player_ind]->GetCard(table->deck_.GetTopCard());
    table->players_[hand.player_ind]->GetCard(last_card);
    hand.cards_pos.second += 2;

    Hand splited_hand_left = Hand(
        hand.bet, hand.player_ind,
        std::make_pair(hand.cards_pos.first, (hand.cards_pos.first + hand.cards_pos.second) / 2));
    Hand splited_hand_right = Hand(
        hand.bet, hand.player_ind,
        std::make_pair((hand.cards_pos.first + hand.cards_pos.second) / 2, hand.cards_pos.second));
    *table->hands_iterator_ = splited_hand_left;
    table->hands_.insert(table->hands_iterator_, splited_hand_right);
}

void SplitIfDoubleAction(BlackjackTable* table, Hand& hand) {
    if (!table->settings_.das) {
        table->players_[hand.player_ind]->ChangeGameStatus();
        return;
    }

    if (!table->players_[hand.player_ind]->PerformBet(3 * hand.bet)) {
        table->players_[hand.player_ind]->ChangeGameStatus();
        return;
    }

    SplitAction(table, hand);
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

        hands_.emplace_back(Hand(settings_.min_bet * (players_[ind]->PerformBet(settings_.min_bet)),
                                 ind, std::make_pair(0, 2)));
    }
    hands_iterator_ = ++hands_.begin();
}

void BlackjackTable::RestartGame() {
    if (players_.empty()) {
        throw std::logic_error("attemt to restart game at the empty desck");
    }

    size_t dealer_score = GetBestPlayerScore(players_, *hands_.begin());
    std::vector<Card> dealer_cards = players_[0]->TakeAllCards();

    deck_.ReturnCards(dealer_cards);

    hands_iterator_ = ++hands_.begin();
    while (hands_iterator_ != hands_.end()) {
        size_t player_score = GetBestPlayerScore(players_, *hands_iterator_);
        size_t player_ind = hands_iterator_->player_ind;

        players_[player_ind]->GetMoney(hands_iterator_->bet *
                                       (player_score >= dealer_score && player_score <= 21));
        players_[player_ind]->GetMoney(hands_iterator_->bet *
                                       (player_score > dealer_score && player_score <= 21));
        ++hands_iterator_;
    }

    for (size_t player_ind = 1; player_ind < players_.size(); player_ind++) {
        std::vector<Card> cards = players_[player_ind]->TakeAllCards();
        deck_.ReturnCards(cards);
    }

    whose_move_ = 1;
    hands_.clear();
}

void BlackjackTable::GameIteration() {
    if (hands_iterator_ == hands_.end() || hands_.empty()) {
        whose_move_ = players_.size();
        return;
    }

    if (!hands_iterator_->bet || GetBestPlayerScore(players_, *hands_iterator_) >= 21) {
        ++hands_iterator_;
        return;
    }

    BlackjackAction player_decision =
        players_[hands_iterator_->player_ind]->BlackjackAction(players_, *hands_iterator_);
    kBlackjackActionsTable.at(player_decision)(this, *hands_iterator_);
    if (!players_[hands_iterator_->player_ind]->GetGameStatus()) {
        players_[hands_iterator_->player_ind]->ChangeGameStatus();
        ++hands_iterator_;
    }
}
