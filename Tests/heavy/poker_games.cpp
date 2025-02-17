#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "../../Tables/poker_table.hpp"

TEST(PokerGame, TwoPlayers) {
    TSQueue<json> logs;
    TSQueue<json> render_queue;
    PokerTable table(logs, render_queue);
    table.AddPlayer(std::make_shared<HumbleGambler>(0, GameType::Poker, false, 1000));
    table.AddPlayer(std::make_shared<HumbleGambler>(0, GameType::Poker, false, 2000));

    size_t games = 1000000;
    for (size_t ind = 0; ind < games; ind++) {
        table.GameIteration();
    }
}
