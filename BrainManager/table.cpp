#include "table.h"
#include "gambler.h"
#include <algorithm>
#include <memory>

void Table::ReshuffleDeck() {
    std::shuffle(deck_.begin(), deck_.end(), random_generator_);
}

int RateCard(Card card, int AceValue) {
    int value = static_cast<int>(card.GetValue());
    if (value <= 10) {
        return value;
    }

    if (value == kCardValues - 1) {
        return AceValue;
    }

    return 10;
}

int GetPlayerScore(std::shared_ptr<Gambler> player) {
    int total_score = 0;
    auto cards = player->ShowCards();
    for (auto card : cards) {
        total_score += RateCard(card, 1);
    }

    return total_score;
}

void BlackjackTable::OneStep() {
    // No players at the table currently
    if (players_.size() == 1) {
        return;
    }

    auto current_player = GetNextPlayer();

    if (start_phase_) {
        Card top_card = GetTopCard();
        players_[whose_move_]->GiveCard(top_card);
    } else {

        while (current_player->GetGameStatus() &&
               !current_player->BlackjackAction()) {
            current_player->GiveCard(GetTopCard());
            if (GetPlayerScore(current_player) >= 21) {
                current_player->ChangeGameStatus();
            }
        }
    }

    ++whose_move_;
    whose_move_ %= players_.size();

    if (!start_phase_ && !whose_move_) {
        start_phase_ ^= 1;
        return;
    }

    if (start_phase_ && !whose_move_ &&
        players_[whose_move_]->ShowCards().size() == 2) {
        start_phase_ ^= 1;
        ++whose_move_;
        whose_move_ %= players_.size();
    }
}

void BlackjackTable::GameItaration() {
    while (start_phase_) {
        OneStep();
    }

    while (!start_phase_) {
        OneStep();
    }
}
