// "Copyright [2024] Netsiaruk Uladzislau"
#ifndef CASINOOBSERVER_BRAINMANAGER_TABLE_H
#define CASINOOBSERVER_BRAINMANAGER_TABLE_H

#include <memory>
#include <random>
#include <vector>

#include "../card.h"
#include "constants.h"
#include "gambler.h"

// TODO: Разнести столы по файлам

class ITable {
public:
    virtual ~ITable() {}

    virtual void ReshuffleDeck() = 0;

    virtual void GameIteration() = 0;

    virtual void OneStep() = 0;

    virtual void AddPlayer(const Gambler &player);
    virtual void RemovePlayer(const Gambler &player);
};

class AbstractITable : public ITable {
public:
    AbstractITable()
        : whose_move_{0}, start_phase_{true},
          random_generator_{std::random_device{}()} {}

protected:
    GameType table_type_;
    std::mt19937 random_generator_;
    bool start_phase_; // This flag indiactes is game on the table just started,
                       // it made for convinient card distribution animation
                       // implementation
    size_t whose_move_; // index of the active player
};

class PokerTable : public AbstractITable {
public:
private:
};

// Player by zero index is always a dealer
class BlackjackTable : public AbstractITable {
public:
    BlackjackTable() : AbstractITable() {}

    void ReshuffleDeck() override;

    void GameIteration() override;

    void OneStep() override;

private:
    Card GetTopCard();

    // returns first player in the order that still in game
    std::shared_ptr<Gambler> GetNextPlayer();

private:
    std::vector<Card> deck_;
    std::vector<std::shared_ptr<Gambler>> players_;
};

#endif // !CASINOOBSERVER_BRAINMANAGER_TABLE_H
