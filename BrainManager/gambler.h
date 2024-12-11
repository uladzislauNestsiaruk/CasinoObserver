#ifndef CASINOOBSERVER_BRAINMANAGER_GAMBLER_H
#define CASINOOBSERVER_BRAINMANAGER_GAMBLER_H

#include <cstdint>
#include <vector>

#include "../card.h"
#include "constants.h"

class Gambler {
  public:
    Gambler()
        : skill_{0}, game_type_{Blackjack}, still_in_game{true}, money_{0} {}

    virtual bool BlackjackAction() = 0;

    virtual void PokerAction() = 0;

    void GiveCard(Card card) { cards_.emplace_back(card); }

    const std::vector<Card> &ShowCards() { return cards_; }

    std::vector<Card> TakeAllCards() {
        std::vector<Card> return_value(cards_);
        cards_.clear();
        return return_value;
    }

    bool GetGameStatus() { return still_in_game; }

    void ChangeGameStatus() { still_in_game ^= 1; }

    virtual ~Gambler() {}

  private:
    int16_t skill_;
    GameType game_type_;
    bool still_in_game;
    size_t money_;
    std::vector<Card> cards_;
};

class HumbleGambler : public Gambler {
  public:
};

class CheaterGambler : public Gambler {
  public:
  private:
    std::vector<Card> visible_cards_;
};

#endif // !CASINOOBSERVER_BRAINMANAGER_GAMBLER_H
