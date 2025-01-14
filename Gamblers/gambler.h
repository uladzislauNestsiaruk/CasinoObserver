#ifndef CASINOOBSERVER_BRAINMANAGER_GAMBLER_H
#define CASINOOBSERVER_BRAINMANAGER_GAMBLER_H

#include <cstdint>
#include <memory>
#include <vector>

// "Copyright [2024] Netsiaruk Uladzislau"
#include "card.hpp"
#include "constants.h"

// Class that describes player state in Blackjack
class Hand;

class IGambler {
public:
    IGambler() {}

    virtual ~IGambler() {}

    virtual BlackjackAction BlackjackAction(const std::vector<std::shared_ptr<IGambler>>&,
                                            const Hand&) = 0;
    virtual PokerMoveState PokerAction(size_t num_opponents, const std::vector<Card>& table_cards,
                                       const std::vector<Card>& hand, size_t current_bet,
                                       size_t min_bet, size_t min_raise, size_t num_raises) = 0;

    virtual bool GetGameStatus() const = 0;
    virtual void ChangeGameStatus() = 0;

    virtual void GetCard(Card card) = 0;
    virtual Card ReturnOneCard() = 0;
    virtual std::vector<Card> TakeAllCards() = 0;
    virtual const std::vector<Card>& ShowCards() const = 0;

    virtual size_t GetBalance() const = 0;
    virtual bool PerformBet(size_t amount) = 0; // return true if Gambler can perform bet and
                                                // substracts amount from his money
    virtual void GetMoney(size_t amount) = 0;   // literally our goal
};

class BaseGambler : public IGambler {
public:
    BaseGambler() : skill_{0}, game_type_{GameType::Blackjack}, still_in_game_{true}, money_{0} {}

    BaseGambler(int16_t skill, GameType game_type, bool still_in_game, size_t money)
        : skill_(skill), game_type_(game_type), still_in_game_(still_in_game), money_(money),
          cards_() {}

    // game status equal true if player did't fold(that's for poker)
    // or didn't stand(thats's for blackjack)
    bool GetGameStatus() const override { return still_in_game_; }
    void ChangeGameStatus() override { still_in_game_ ^= 1; }

    void GetCard(Card card) override { cards_.emplace_back(card); }
    Card ReturnOneCard() override {
        Card last_card = cards_.back();
        cards_.pop_back();
        return last_card;
    }
    const std::vector<Card>& ShowCards() const override { return cards_; }
    std::vector<Card> TakeAllCards() override;

    size_t GetBalance() const override { return money_; }

    bool PerformBet(size_t amount) override {
        if (amount > money_) {
            return false;
        }

        money_ -= amount;
        return true;
    }

    void GetMoney(size_t amount) override { money_ += amount; }

    ~BaseGambler() override {}

private:
    int16_t skill_;
    GameType game_type_;
    bool still_in_game_;
    size_t money_;
    std::vector<Card> cards_;
};

class HumbleGambler : public BaseGambler {
public:
    HumbleGambler() : BaseGambler() {}

    HumbleGambler(int16_t skill, GameType game_type, bool still_in_game, size_t money)
        : BaseGambler(skill, game_type, still_in_game, money) {}

    enum BlackjackAction BlackjackAction(const std::vector<std::shared_ptr<IGambler>>&,
                                         const Hand&) override;

    PokerMoveState PokerAction(size_t num_opponents, const std::vector<Card>& table_cards,
                               const std::vector<Card>& hand, size_t current_bet, size_t min_bet,
                               size_t min_raise, size_t num_raises) override;

    ~HumbleGambler() override {}
};

class CheaterGambler : public BaseGambler {
public:
private:
    std::vector<Card> visible_cards_;
};

#endif // !CASINOOBSERVER_BRAINMANAGER_GAMBLER_H
