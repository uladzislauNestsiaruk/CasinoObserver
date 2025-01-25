#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <memory>
#include <optional>
#include <queue>

#include "../../Tables/blackjack_table.h"
#include "constants.h"
/*
struct TableOutput {
    TSQueue<json> logs_;
    TSQueue<json> render_queue_;
};

struct PlayerInfo {
    struct Hand {
        size_t bet;
        std::vector<int> cards;
    };

    size_t balance;
    std::vector<Hand> hands;
};

Card CardByValue(uint8_t value) {
    return Card(static_cast<CardType>(value >> kValueBits),
                static_cast<CardValue>((value << (8 - kValueBits)) >> (8 - kValueBits)));
}

using blackjack_callback_input_t = std::pair<PlayerInfo&, json&>;
using blackjack_callback_t = void (*)(blackjack_callback_input_t);

bool BetRaise(blackjack_callback_input_t info, std::string trigger) {
    if (info.second.find(trigger) != info.second.end()) {
        EXPECT_GE(info.first.balance, info.first.hands.back().bet);
        info.first.balance -= info.first.hands.back().bet;
        return true;
    }

    return false;
}

void HitActionTest(blackjack_callback_input_t info) {
    info.first.hands[0].cards.push_back(info.second["card1"]);
}

void StandActionTest(blackjack_callback_input_t info) {}

void DoubleOrHitActionTest(blackjack_callback_input_t info) {
    BetRaise(info, "doubled");
    HitActionTest(info);
}

void DoubleOrStandActionTest(blackjack_callback_input_t info) {
    if (BetRaise(info, "doubled")) {
        HitActionTest(info);
    }
}

void SplitActionTest(blackjack_callback_input_t info) {
    if (!BetRaise(info, "splited")) {
        return;
    }

    ASSERT_EQ(info.first.hands.back().cards.size(), 2);
    PlayerInfo::Hand hand = info.first.hands.back();
    info.first.hands.pop_back();
    info.first.hands.push_back(PlayerInfo::Hand{
        hand.bet, std::vector<int>{info.second["hand2"].get<std::pair<int, int>>().first,
                                   info.second["hand2"].get<std::pair<int, int>>().second}});
    info.first.hands.push_back(PlayerInfo::Hand{
        hand.bet, std::vector<int>{info.second["hand1"].get<std::pair<int, int>>().first,
                                   info.second["hand1"].get<std::pair<int, int>>().second}});
}

void SplitIfDoubleActionTest(blackjack_callback_input_t info) {
    if (BetRaise(info, "doubled")) {
        SplitActionTest(info);
    }
}

const std::unordered_map<BlackjackAction, blackjack_callback_t> kCallbacksMap = {
    {BlackjackAction::HIT, HitActionTest},
    {BlackjackAction::STAND, StandActionTest},
    {BlackjackAction::DOUBLE_OR_HIT, DoubleOrHitActionTest},
    {BlackjackAction::DOUBLE_OR_STAND, DoubleOrStandActionTest},
    {BlackjackAction::SPLIT, SplitActionTest},
    {BlackjackAction::SPLIT_IF_DOUBLE, SplitIfDoubleActionTest}};

std::queue<json> GetGame(TableOutput& output) {
    std::queue<json> game;
    game.push(output.logs_.pop().value());
    std::optional<json> last = output.logs_.pop();
    output.logs_.push(last.value());

    while (last && (last.value().find("phase") == last.value().end() ||
                    last.value()["phase"] != "dealing")) {
        game.push(last.value());
        output.logs_.pop();
        last = output.logs_.pop();
        output.logs_.push(last.value());
    }

    return game;
}

void CheckDealing(std::queue<json>& game, std::vector<PlayerInfo>& players) {
    ASSERT_EQ(game.front()["phase"], "dealing");
    game.pop();

    while (!game.empty() && game.front().find("phase") == game.front().end()) {
        ASSERT_EQ(players.size(), game.front()["player"]);

        players.emplace_back(
            PlayerInfo{game.front()["balance"],
                       std::vector<PlayerInfo::Hand>(
                           1, {game.front()["bet"],
                               std::vector<int>{game.front()["card1"], game.front()["card2"]}})});

        game.pop();
    }
}

void CheckMoves(std::queue<json>& game, std::vector<PlayerInfo>& players) {
    ASSERT_EQ(game.front()["phase"], "game");
    game.pop();

    size_t player_ind = 1;
    while (!game.empty() && game.front().find("phase") == game.front().end()) {
        ASSERT_LE(player_ind, game.front()["player"]);
        ASSERT_LT(game.front()["score"], 21);

        BlackjackAction action = game.front()["action"];
        kCallbacksMap.at(action)({players[player_ind], game.front()});

        game.pop();
    }
}

void CheckWinnersSelection(std::queue<json>& game, std::vector<PlayerInfo>& players) {
    ASSERT_EQ(game.front()["phase"], "winners selection");
    game.pop();
}

void CheckGame(std::queue<json>& game) {
    ASSERT_GT(game.size(), 3);

    std::vector<PlayerInfo> players;
    CheckDealing(game, players);
    CheckMoves(game, players);
    CheckWinnersSelection(game, players);
}

void CheckCorrectness(TableOutput& output) {
    while (!output.logs_.empty()) {
        std::queue<json> game = GetGame(output);
        CheckGame(game);
    }
}

TEST(ExceptionCheck, MultiplePlayers) {
    TableOutput output;
    BlackjackTable table(output.logs_, output.render_queue_);

    size_t games = 10000;

    for (size_t players = 0; players < 8; players++) {
        table.AddPlayer(std::make_shared<HumbleGambler>(100, GameType::Blackjack, true, 10000));
        for (size_t ind = 0; ind < games; ind++) {
            table.GameIteration();
        }
    }
}

TEST(GameCorrectness, MultiplePlayers) {
    TableOutput output;
    BlackjackTable table(output.logs_, output.render_queue_);

    size_t games = 10000;

    for (size_t players = 0; players < 8; players++) {
        table.AddPlayer(std::make_shared<HumbleGambler>(100, GameType::Blackjack, true, 10000));
        for (size_t ind = 0; ind < games; ind++) {
            table.GameIteration();
        }
    }

    CheckCorrectness(output);
}
*/
