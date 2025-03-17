#include <iostream>
#include <memory>
#include <numeric>
#include <stdexcept>
#include <string>
#include <utility>

#include "blackjack_table.hpp"
#include "common.hpp"
#include "constants.hpp"

std::string GetPhaseByCard(Card card) {
    uint8_t type = static_cast<uint8_t>(card.GetType());
    uint8_t value = static_cast<uint8_t>(card.GetValue());
    return value_names[value] + "_of_" + type_names[type];
}

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

int GetBestPlayerScore(const std::vector<Card>& player_cards, const Hand& hand) {
    size_t score = std::accumulate(player_cards.begin() + hand.cards_pos.first,
                                   player_cards.begin() + hand.cards_pos.second, 0,
                                   [&](size_t cur, Card card) { return cur + RateCard(card, 11); });

    for (size_t ind = hand.cards_pos.first; ind < hand.cards_pos.second; ind++) {
        if (score <= 21) {
            break;
        }

        score += RateCard(player_cards[ind], 1) - RateCard(player_cards[ind], 11);
    }

    return score;
}

BlackjackAction HumbleGambler::BlackjackAction(const std::vector<Card>& dealer_hand,
                                               const Hand& hand) {
    const std::vector<Card>& cards = ShowCards();
    size_t total_cards = hand.cards_pos.second - hand.cards_pos.first;

    Card dealer_open = dealer_hand[0];
    size_t player_score = GetBestPlayerScore(cards, hand);

    if (total_cards == 2 &&
        RateCard(cards[hand.cards_pos.first]) == RateCard(cards[hand.cards_pos.second])) {
        return kPairSplitTable[RateCard(cards[hand.cards_pos.first])][RateCard(dealer_open)];
    } else if (total_cards == 2 && (cards[hand.cards_pos.first].GetValue() == CardValue::Ace ||
                                    cards[hand.cards_pos.second].GetValue() == CardValue::Ace)) {
        int non_soft_total = RateCard(cards[hand.cards_pos.first], 1) +
                             RateCard(cards[hand.cards_pos.second], 1) - 1;
        return kSoftTotalsTable[non_soft_total][RateCard(dealer_open)];
    }
    return kHardTotalsTable[player_score][RateCard(dealer_open)];
}

void DummyAction(BlackjackTable* table, Hand& hand, json&) {
    throw std::logic_error("Something went wrong, hello from DummyAction");
}

void HitAction(BlackjackTable* table, Hand& hand, json& log) {
    const auto player = table->players_[hand.player_ind];
    player->GetCard(table->deck_.GetTopCard());
    log["card1"] = player->ShowCards()[0].GetValue();
    ++hand.cards_pos.second;
}

void StandAction(BlackjackTable* table, Hand& hand, json& log) {
    table->players_[hand.player_ind]->ChangeGameStatus();
}

void DoubleOrHitAction(BlackjackTable* table, Hand& hand, json& log) {
    auto player = table->players_[hand.player_ind];
    if (player->PerformBet(hand.bet)) {
        hand.bet <<= 1;
        log["doubled"];
        player->ChangeGameStatus();
    }

    player->GetCard(table->deck_.GetTopCard());
    log["card1"] = player->ShowCards().back().GetValue();
    ++hand.cards_pos.second;
}

void DoubleOrStandAction(BlackjackTable* table, Hand& hand, json& log) {
    auto player = table->players_[hand.player_ind];
    if (player->PerformBet(hand.bet)) {
        hand.bet <<= 1;
        player->GetCard(table->deck_.GetTopCard());
        log["doubled"];
        log["card1"] = player->ShowCards().back().GetValue();
        ++hand.cards_pos.second;
    }

    player->ChangeGameStatus();
}

void SplitAction(BlackjackTable* table, Hand& hand, json& log) {
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

    const std::vector<Card>& player_cards = table->players_[hand.player_ind]->ShowCards();
    log["splited"];
    log["hand1"] =
        std::make_pair(player_cards[hand.cards_pos.first].GetValue(),
                       player_cards[(hand.cards_pos.first + hand.cards_pos.second) / 2].GetValue());
    log["hand2"] =
        std::make_pair(player_cards[(hand.cards_pos.first + hand.cards_pos.second) / 2].GetValue(),
                       player_cards[hand.cards_pos.second].GetValue());

    *table->hands_iterator_ = splited_hand_right;
    table->hands_.insert(table->hands_iterator_, splited_hand_left);
}

void SplitIfDoubleAction(BlackjackTable* table, Hand& hand, json& log) {
    if (!table->settings_.das) {
        table->players_[hand.player_ind]->ChangeGameStatus();
        return;
    }

    if (!table->players_[hand.player_ind]->PerformBet(3 * hand.bet)) {
        table->players_[hand.player_ind]->ChangeGameStatus();
        return;
    }
    log["doubled"];

    SplitAction(table, hand, log);
}

BlackjackTable::BlackjackTable(TSQueue<json>& logs, TSQueue<json>& render_queue)
    : AbstractTable(GameType::Blackjack, render_queue), is_game_finished_(true), hands_(),
      settings_{100, true}, logs_(logs), render_queue_(render_queue) {
    occupied_places_.fill(false);
    GenPlayers(kGamblersPlaces, kGamblersPlaces - 1, GameType::Blackjack);
}

bool BlackjackTable::IsGameFinished() const { return is_game_finished_.load(); }

void BlackjackTable::AddDealingPhase(const std::string& phase, size_t delay) {
    json render_event;
    render_event["event"]["type"] = "change_phase";
    render_event["new_phase"] = phase;
    render_event["tag"] = "root";
    render_event["delay"] = delay;
    AddRenderEvent(render_event);
}

void BlackjackTable::AddCardPhase(Card card, size_t card_id, size_t delay) {
    json render_event;
    render_event["event"]["type"] = "change_phase";
    render_event["new_phase"] = GetPhaseByCard(card);
    if (!card_id) {
        render_event["tag"] = "dealer_1";
    } else {
        render_event["tag"] =
            "player_" + std::to_string((card_id + 1) / 2) + "_" + std::to_string(2 - (card_id & 1));
    }
    render_event["delay"] = delay;
    AddRenderEvent(render_event);
}

void BlackjackTable::Dealing() {
    deck_.ReshuffleDeck();
    logs_.push({{"phase", "dealing"}});

    AddDealingPhase("afk", default_antimation_end_delay);

    for (size_t ind = 0; ind < players_.size(); ind++) {
        players_[ind]->GetCard(deck_.GetTopCard());
        players_[ind]->GetCard(deck_.GetTopCard());

        size_t player_bet = settings_.min_bet * (players_[ind]->PerformBet(settings_.min_bet));
        hands_.emplace_back(Hand(player_bet, ind, std::make_pair(0, 2)));

        logs_.push({{"player", ind},
                    {"card1", players_[ind]->ShowCards()[0].GetValue()},
                    {"card2", players_[ind]->ShowCards()[1].GetValue()},
                    {"bet", player_bet},
                    {"balance", players_[ind]->GetBalance()}});
    }

    for (size_t ind = 0; ind < players_.size(); ind++) {
        AddDealingPhase("players_dealing_" + std::to_string(2 * ind + 1), default_delay);
        if (IsPlaceOccupied(ind)) {
            AddCardPhase(players_[ind]->ShowCards()[0], 2 * ind + 1, default_delay);
        }
        AddDealingPhase("players_dealing_" + std::to_string(2 * ind + 2), default_delay);
        if (IsPlaceOccupied(ind)) {
            AddCardPhase(players_[ind]->ShowCards()[1], 2 * ind + 2, default_delay);
        }
    }

    dealer_cards_.emplace_back(deck_.GetTopCard());
    dealer_cards_.emplace_back(deck_.GetTopCard());

    AddDealingPhase("dealer_dealing_1", default_delay);
    AddCardPhase(dealer_cards_[0], 0, default_delay);
    AddDealingPhase("dealer_dealing_2", default_delay);

    hands_iterator_ = ++hands_.begin();
}

void BlackjackTable::ClearRollback() {}

void BlackjackTable::Clean() {
    if (players_.empty()) {
        throw std::logic_error("attemt to restart game at the empty desck");
    }

    size_t dealer_score = GetBestPlayerScore(dealer_cards_, *hands_.begin());

    deck_.ReturnCards(dealer_cards_);
    dealer_cards_.clear();
    logs_.push({{"change_phase", "winners_selection"}});

    hands_iterator_ = ++hands_.begin();
    while (hands_iterator_ != hands_.end()) {
        size_t player_score = GetBestPlayerScore(players_[hands_iterator_->player_ind]->ShowCards(),
                                                 *hands_iterator_);
        size_t player_prize =
            hands_iterator_->bet * (player_score >= dealer_score && player_score <= 21) +
            hands_iterator_->bet * (player_score > dealer_score && player_score <= 21);

        players_[hands_iterator_->player_ind]->GetMoney(player_prize);
        logs_.push({{"player", hands_iterator_->player_ind}, {"prize", player_prize}});
        ++hands_iterator_;
    }

    for (size_t player_ind = 0; player_ind < players_.size(); player_ind++) {
        std::vector<Card> cards = players_[player_ind]->ReturnAllCards();
        deck_.ReturnCards(cards);
    }

    whose_move_ = 1;
    hands_.clear();
}

void BlackjackTable::GameIteration() {
    if (players_.empty()) {
        return;
    }
    is_game_finished_.store(false);
    Dealing();

    while (hands_iterator_ != hands_.end()) {
        size_t current_player_score = GetBestPlayerScore(
            players_[hands_iterator_->player_ind]->ShowCards(), *hands_iterator_);
        if (!hands_iterator_->bet || current_player_score >= 21) {
            ++hands_iterator_;
            continue;
        }

        BlackjackAction player_decision = players_[hands_iterator_->player_ind]->BlackjackAction(
            players_[hands_iterator_->player_ind]->ShowCards(), *hands_iterator_);

        json action_log = {{"player", hands_iterator_->player_ind},
                           {"score", current_player_score},
                           {"action", player_decision}};
        kBlackjackActionsTable.at(player_decision)(this, *hands_iterator_, action_log);
        if (!players_[hands_iterator_->player_ind]->GetGameStatus()) {
            players_[hands_iterator_->player_ind]->ChangeGameStatus();
            ++hands_iterator_;
        }

        logs_.push(action_log);
    }

    Clean();
    is_game_finished_.store(true);
}
