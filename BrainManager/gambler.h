#ifndef CASINOOBSERVER_BRAINMANAGER_GAMBLER_H
#define CASINOOBSERVER_BRAINMANAGER_GAMBLER_H

#include <cstdint>
#include <vector>

// "Copyright [2024] Netsiaruk Uladzislau"
#include "../card.h"
#include "../utils/constants.h"

class IGambler {
public:
    IGambler() {}

    virtual ~IGambler() {}

    virtual bool BlackjackAction() = 0;
    virtual void PokerAction() = 0;

    virtual bool GetGameStatus() const = 0;
    virtual void ChangeGameStatus() = 0;

    virtual void GetCard(Card card) = 0;
    virtual const std::vector<Card>& ShowCards() const = 0;
    virtual std::vector<Card> TakeAllCards() = 0;
};

class BaseGambler : public IGambler {
public:
    BaseGambler() : skill_{0}, game_type_{GameType::Blackjack}, still_in_game{true}, money_{0} {}

    bool GetGameStatus() const override { return still_in_game; }
    void ChangeGameStatus() override { still_in_game ^= 1; }

    void GetCard(Card card) override { cards_.emplace_back(card); }
    const std::vector<Card>& ShowCards() const override { return cards_; }
    std::vector<Card> TakeAllCards() override;

private:
    int16_t skill_;
    GameType game_type_;
    bool still_in_game;
    size_t money_;
    std::vector<Card> cards_;
};

class HumbleGambler : public BaseGambler {
public:
};

class CheaterGambler : public BaseGambler {
public:
private:
    std::vector<Card> visible_cards_;
};

#endif // !CASINOOBSERVER_BRAINMANAGER_GAMBLER_H
