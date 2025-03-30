#include <animations_manager.hpp>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "../../tables/poker_table.hpp"

TEST(PokerGame, TwoPlayers) {
    TSQueue<json> logs;
    TSQueue<json> render_queue;
    AnimationsManager animations_manager_;
    PokerTable table(logs, render_queue, animations_manager_);
    table.AddPlayer(
        std::make_shared<HumbleGambler>(0, 0, GameType::Poker, false, 1000, "dummy_tag"));
    table.AddPlayer(
        std::make_shared<HumbleGambler>(0, 0, GameType::Poker, false, 2000, "dummy_tag"));

    size_t games = 1000000;
    for (size_t ind = 0; ind < games; ind++) {
        table.GameIteration();
    }
}
