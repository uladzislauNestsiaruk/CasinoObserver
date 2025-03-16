#include "poker_table.hpp"
#include <common.hpp>
#include <constants.hpp>
#include <cstdint>
#include <iostream>

namespace {
constexpr uint32_t num_iterations = 400;
const std::array<std::string, 4> type_names = {"hearts", "clubs", "diamonds", "spades"};
const std::array<std::string, 13> value_names = {"two",   "three", "four", "five", "six",
                                                 "seven", "eight", "nine", "ten",  "jack",
                                                 "queen", "king",  "ace"};

struct PokerComboInfo {
    int combo_id;
    std::vector<CardValue> values;

    bool operator==(const PokerComboInfo& other_combo) const {
        return std::tie(combo_id, values) == std::tie(other_combo.combo_id, other_combo.values);
    }

    bool operator>(const PokerComboInfo& other_combo) const {
        return std::tie(combo_id, values) > std::tie(other_combo.combo_id, other_combo.values);
    }

    bool operator<(const PokerComboInfo& other_combo) const {
        return std::tie(combo_id, values) < std::tie(other_combo.combo_id, other_combo.values);
    }
};

// Rate introduced by pair <id of combination increasing by priority, card values which
// characterized this combination>
PokerComboInfo RateHand(const std::vector<Card>& table_cards, const std::vector<Card>& hand) {
    std::vector<Card> all_cards(table_cards);
    all_cards.insert(all_cards.end(), hand.begin(), hand.end());

    std::unordered_map<CardType, std::vector<CardValue>> type_map;
    std::unordered_map<CardValue, size_t> value_map;
    for (const auto& card : all_cards) {
        type_map[card.GetType()].push_back(card.GetValue());
        ++value_map[card.GetValue()];
    }

    CardValue greater_card;
    std::vector<CardValue> pairs;
    std::vector<CardValue> three_of_a_kind;
    std::vector<CardValue> four_of_a_kind;
    CardValue straight;
    std::vector<CardValue> flash;
    std::vector<CardValue> straight_flash;
    // Check flash and straight flash
    for (auto& cards : type_map) {
        if (cards.second.size() < 5) {
            continue;
        }

        std::sort(cards.second.begin(), cards.second.end());
        std::reverse(cards.second.begin(), cards.second.end());
        std::vector<CardValue> five_highest(cards.second.begin(), cards.second.begin() + 5);
        flash = std::max(flash, five_highest);
        for (size_t i = 0; i + 4 < cards.second.size(); ++i) {
            if (static_cast<uint8_t>(cards.second[i]) - 4 ==
                static_cast<uint8_t>(cards.second[i + 4])) {
                std::vector<CardValue> temp(cards.second.begin() + i, cards.second.begin() + i + 5);
                straight_flash = std::max(straight_flash, temp);
                break;
            }
        }
    }
    // Finding all pairs, sets, straights and fours of a kind
    for (auto& value : value_map) {
        if (value.second == 4) {
            four_of_a_kind.push_back(value.first);
            three_of_a_kind.push_back(value.first);
            pairs.push_back(value.first);
            pairs.push_back(value.first);
        } else if (value.second == 3) {
            three_of_a_kind.push_back(value.first);
            pairs.push_back(value.first);
        } else if (value.second == 2) {
            pairs.push_back(value.first);
        } else if (value.second == 1) {
            greater_card = std::max(greater_card, value.first);
        }

        bool no_straight = false;
        for (size_t i = 1; i < 5; ++i) {
            no_straight |=
                !value_map.contains(static_cast<CardValue>(static_cast<uint8_t>(value.first) - i));
        }

        if (!no_straight) {
            straight = std::max(straight, value.first);
        }
    }

    if (!straight_flash.empty()) {
        return {(straight_flash[0] == CardValue::Ace ? 10 : 9), {straight_flash[0]}};
    } else if (!four_of_a_kind.empty()) {
        std::sort(four_of_a_kind.begin(), four_of_a_kind.end());
        return {8, {four_of_a_kind.back()}};
    } else if (!three_of_a_kind.empty() && !pairs.empty()) {
        std::sort(three_of_a_kind.begin(), three_of_a_kind.end());
        std::sort(pairs.begin(), pairs.end());
        if (pairs.size() == 1 && three_of_a_kind.size() == 1) {
            return {4, {three_of_a_kind.back()}};
        }

        return {
            7,
            {three_of_a_kind.back(),
             (pairs.back() != three_of_a_kind.back() ? pairs.back() : pairs[pairs.size() - 2])}};
    } else if (!flash.empty()) {
        return {6, flash};
    } else if (static_cast<uint8_t>(straight) != 0) {
        return {5, {straight}};
    } else if (!three_of_a_kind.empty()) {
        std::sort(three_of_a_kind.begin(), three_of_a_kind.end());
        return {4, {three_of_a_kind.back()}};
    } else if (!pairs.empty()) {
        std::sort(pairs.begin(), pairs.end());
        if (pairs.size() > 1) {
            return {3, {pairs.back(), pairs[pairs.size() - 2]}};
        }

        return {2, {pairs.back()}};
    } else {
        return {1, {greater_card}};
    }
}

double WinProbability(size_t num_opponents, const std::vector<Card>& table_cards,
                      const std::vector<Card>& hand) {
    std::vector<Card> all_cards(table_cards);
    all_cards.insert(all_cards.end(), hand.begin(), hand.end());

    Deck exp_deck(true);
    exp_deck.RemoveCards(all_cards);
    uint16_t wins = 0;
    uint16_t ties = 0;

    std::vector<Card> exp_table_cards(table_cards);
    for (size_t i = 0; i < num_iterations; ++i) {
        exp_deck = Deck(true);
        exp_deck.ReshuffleDeck();
        for (size_t j = 0; j < 5 - table_cards.size(); ++j) {
            exp_table_cards.push_back(exp_deck.GetTopCard());
        }

        bool at_least_tie = true;
        bool win = true;

        PokerComboInfo hand_rate = RateHand(exp_table_cards, hand);
        for (size_t j = 0; j < num_opponents; ++j) {
            PokerComboInfo opponent_rate =
                RateHand(exp_table_cards, {exp_deck.GetTopCard(), exp_deck.GetTopCard()});
            if (opponent_rate == hand_rate) {
                win = false;
            } else if (opponent_rate > hand_rate) {
                win = at_least_tie = false;
                break;
            }
        }

        if (win) {
            wins += win;
        } else if (at_least_tie) {
            ties += at_least_tie;
        }
    }

    return static_cast<double>(wins + 0.5 * ties) / num_iterations;
}
std::string GetPhaseByCard(Card card) {
    uint8_t type = static_cast<uint8_t>(card.GetType());
    uint8_t value = static_cast<uint8_t>(card.GetValue());
    return value_names[value] + "_of_" + type_names[type];
}
} // namespace

PokerTable::PokerTable(TSQueue<json>& logs, TSQueue<json>& render_queue)
    : AbstractTable(GameType::Poker, render_queue), min_bet_(100), min_raise_(50), small_blind_(50),
      big_blind_(min_bet_), logs_(logs) {

    occupied_places_.fill(false);
    GenPlayers(get_random_number(2, 6), kGamblersPlaces - 1, GameType::Poker);
}

void PokerTable::MakeBet(size_t amount, size_t player_ind) {
    if (players_[player_ind]->PerformBet(amount)) {
        bank_ += amount;
        return;
    }

    throw std::runtime_error("[ERROR]: Bet is more than player bank");
}

void PokerTable::ApplyBets() {
    for (size_t i = 0; i < players_.size(); ++i) {
        if (bets_[i].amount > 0) {
            rollback_logs_.push({{"player_id", i}, {"amount", bets_[i].amount}});
            MakeBet(bets_[i].amount, i);
        }
    }
}

void PokerTable::Dealing() {
    // Not enough players
    if (players_.size() < 2) {
        return;
    }

    is_all_in_.assign(players_.size(), 0);
    for (auto player : players_) {
        if (!player->GetGameStatus()) {
            player->ChangeGameStatus();
        }
    }

    bets_.assign(players_.size(), {0, 0});
    deck_.ReshuffleDeck();
    size_t cnt = 0;
    for (int i = 0; i < players_.size(); ++i) {
        if (players_[i]->GetBalance() < big_blind_) {
            players_[i]->ChangeGameStatus();
            RemovePlayer(i);
            --i;
            continue;
        }
        cnt += players_[i]->GetBalance();
        bets_[i] = {big_blind_, 0};
        ++active_players_;
    }

    // Not enough active players
    if (active_players_ < 2) {
        return;
    }

    ApplyBets();
}

PokerMoveState HumbleGambler::PokerAction(size_t num_opponents,
                                          const std::vector<Card>& table_cards,
                                          const std::vector<Card>& hand, size_t current_bet,
                                          size_t min_bet, size_t min_raise, size_t num_raises) {
    // First Move (Blinds)
    if (table_cards.size() + hand.size() == 0) {
        return {.move = PokerMove::CALL, .money_amount = current_bet};
    }

    double probability = WinProbability(num_opponents, table_cards, hand);

    double raise = 0.0;
    if (probability > 0.85) {
        raise = (current_bet == 0 ? min_bet * 2.0 : current_bet * 1.5);
    } else if (probability > 0.7) {
        raise = (current_bet == 0 ? min_bet * 1.5 : current_bet * 1.0);
    } else if (probability > 0.55) {
        raise = (current_bet == 0 ? min_bet * 1.25 : current_bet * 0.5);
    } else if (probability > 0.4) {
        raise = (current_bet == 0 ? min_bet * 1.0 : 0.0);
    } else if ((probability < 0.3 && table_cards.size() == 5) ||
               (probability < 0.2 && table_cards.size() == 4) ||
               (probability < 0.1 && table_cards.size() == 3) ||
               (probability < 0.05 && table_cards.empty())) {
        return {.move = PokerMove::FOLD};
    }

    if (num_raises == 2) {
        raise = 0.0;
    }

    size_t total_bet =
        current_bet + (raise != 0 && raise < min_raise ? min_raise : static_cast<size_t>(raise));

    if (total_bet > 0.95 * GetBalance()) {
        if (GetBalance() < min_bet) {
            return {.move = PokerMove::FOLD};
        }

        return {.move = PokerMove::ALL_IN, .money_amount = GetBalance()};
    }

    if (total_bet > current_bet) {
        return {.move = (current_bet == 0 ? PokerMove::BET : PokerMove::RAISE),
                .money_amount = total_bet};
    }

    return {.move = PokerMove::CALL, .money_amount = current_bet};
}

void PokerTable::DistributionPhase(std::string_view phase) {
    if (phase == "preflop") {
        for (auto player : players_) {
            player->GetCard(deck_.GetTopCard());
            player->GetCard(deck_.GetTopCard());
        }
    } else if (phase == "flop") {
        for (const std::string& card_id : {"first", "second", "third"}) {
            Card card = deck_.GetTopCard();
            json render_event;
            render_event["event"]["type"] = "change_phase";
            render_event["new_phase"] = GetPhaseByCard(card);
            render_event["tag"] = card_id + "_central_card";
            render_event["delay"] = default_antimation_end_delay;
            render_queue_.push(render_event);

            table_cards_.push_back(card);
        }
    } else {
        Card card = deck_.GetTopCard();
        json render_event;
        render_event["event"]["type"] = "change_phase";
        render_event["new_phase"] = GetPhaseByCard(card);
        render_event["tag"] = (phase == "turn" ? "fourth_central_card" : "fivth_central_card");
        render_event["delay"] = default_antimation_end_delay;
        render_queue_.push(render_event);
        table_cards_.push_back(card);
    }
}

void PokerTable::BettingPhase() {
    size_t moves_cnt = 0;
    for (size_t ind = 0;
         bets_[ind].amount != current_bet_ || (current_bet_ == 0 && moves_cnt < players_.size());
         ind = (ind + 1) % players_.size()) {
        if (was_action_performed_.load()) {
            return;
        }

        if (active_players_ == all_in_players_) {
            show_all_cards_ = 1;
            break;
        }

        ++moves_cnt;
        if (players_[ind]->GetGameStatus() && !is_all_in_[ind]) {
            if (active_players_ == 1) {
                show_all_cards_ = 1;
                return;
            }

            PokerMoveState move_state = players_[ind]->PokerAction(
                players_.size() - 1, table_cards_, players_[ind]->ShowCards(), current_bet_,
                min_bet_, min_raise_, bets_[ind].num_raises);
            json move_log = {{"type", "trade_move"}, {"player_id", ind}};

            switch (static_cast<uint8_t>(move_state.move)) {
            case static_cast<uint8_t>(PokerMove::ALL_IN):
                is_all_in_[ind] = true;
                ++all_in_players_;
                current_bet_ = std::min(current_bet_, move_state.money_amount.value());
                move_log["move"] = "all_in";
                move_log["amount"] = move_state.money_amount.value();
                break;
            case static_cast<uint8_t>(PokerMove::BET):
                current_bet_ = move_state.money_amount.value();
                move_log["move"] = "bet";
                move_log["amount"] = move_state.money_amount.value();
                break;
            case static_cast<uint8_t>(PokerMove::RAISE):
                current_bet_ = move_state.money_amount.value();
                ++bets_[ind].num_raises;
                move_log["move"] = "raise";
                move_log["amount"] = move_state.money_amount.value();
                break;
            case static_cast<uint8_t>(PokerMove::CALL):
                move_log["move"] = "call";
                break;
            case static_cast<uint8_t>(PokerMove::FOLD):
                --active_players_;
                players_[ind]->ChangeGameStatus();
                move_log["move"] = "fold";
                break;
            }
            logs_.push(move_log);

            if (move_state.move != PokerMove::FOLD) {
                bets_[ind].amount = move_state.money_amount.value();
            }
        }
    }
}

void PokerTable::SelectWinners() {
    std::vector<std::pair<PokerComboInfo, std::shared_ptr<IGambler>>> hand_rates(players_.size());
    for (size_t i = 0; i < players_.size(); ++i) {
        hand_rates[i] = {RateHand(table_cards_, players_[i]->ShowCards()), players_[i]};
    }
    std::sort(hand_rates.begin(), hand_rates.end(),
              [this](auto lhs, auto rhs) { return lhs > rhs; });

    size_t num_winners = hand_rates.size();
    for (size_t i = 1; i < hand_rates.size(); ++i) {
        if (hand_rates[0].first != hand_rates[i].first) {
            num_winners = i;
            break;
        }
    }
    for (size_t i = 0; i < num_winners; ++i) {
        hand_rates[i].second->GetMoney(bank_ / num_winners);
        logs_.push({{"type", "game_end"}, {"result", "win"}, {"profit", bank_ / num_winners}});
        rollback_logs_.push({{"player_id", i}, {"amount", -bank_ / num_winners}});
    }
    for (size_t i = num_winners; i < players_.size(); ++i) {
        logs_.push({
            {"type", "game_end"}, {"result", "lose"} //{"loss", bank_ / num_winners}
        });
    }
}

void PokerTable::Clean() {
    bank_ = 0;
    current_bet_ = 0;
    show_all_cards_ = 0;
    active_players_ = 0;
    all_in_players_ = 0;

    for (auto player : players_) {
        if (player->GetGameStatus()) {
            player->ChangeGameStatus();
        }

        deck_.ReturnCards(player->TakeAllCards());
    }

    deck_.ReturnCards(table_cards_);
    table_cards_.clear();
    is_active_game_.store(false);
}

void PokerTable::GameIteration() {
    is_active_game_.store(true);
    Dealing();
    if (active_players_ < 2) {
        Clean();
        GenPlayers(get_random_number(1, 6 - players_.size()), kGamblersPlaces - 1, GameType::Poker);
        return;
    }

    for (const auto& [phase, part] : std::vector<std::pair<std::string, int>>{
             {"preflop", 1}, {"flop", 3}, {"turn", 2}, {"river", 2}}) {
        if (was_action_performed_.load()) {
            break;
        }

        current_bet_ = 0;
        bets_.assign(players_.size(), {0, 0});
        DistributionPhase(phase);
        for (size_t i = 1; i <= part; ++i) {
            json render_event;
            render_event["event"]["type"] = "change_phase";
            render_event["new_phase"] = phase + "_" + std::to_string(i);
            render_event["tag"] = "root";
            render_event["delay"] = (i == 1 ? 100 : default_delay);
            AddRenderEvent(render_event);
        }

        if (show_all_cards_) {
            continue;
        }

        BettingPhase();
    }

    SelectWinners();
    Clean();
}

bool PokerTable::IsGameFinished() const { return !is_active_game_.load(); }

void PokerTable::RollbackGame() {
    while (!rollback_logs_.empty()) {
        json log_event = rollback_logs_.front();
        rollback_logs_.pop();
        players_[log_event["player_id"].template get<size_t>()]->GetMoney(
            log_event["amount"].template get<uint64_t>());
    }
}

void PokerTable::ClearRollback() {
    while (!rollback_logs_.empty()) {
        rollback_logs_.pop();
    }
}
