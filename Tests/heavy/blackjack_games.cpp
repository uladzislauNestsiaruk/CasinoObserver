#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "../../Tables/blackjack_table.h"
#include "constants.h"

TEST(BasicFunctionality, OnePlayer) {
    BlackjackTable table;
    table.AddPlayer(new HumbleGambler(0, GameType::Blackjack, true, 1000));

    size_t games = 1000000;
    for (size_t ind = 0; ind < games; ind++) {
        table.Dealing();
        while (!table.IsGameFinished()) {
            table.GameIteration();
        }
      
        table.Clean();
    }
}
