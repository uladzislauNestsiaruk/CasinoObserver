// "Copyright [2024] Netsiaruk Uladzislau"
#ifndef CASINOOBSERVER_BRAINMANAGER_TABLE_H
#define CASINOOBSERVER_BRAINMANAGER_TABLE_H

#include <memory>
#include <vector>

#include "../deck.h"
#include "../utils/constants.h"
#include "gambler.h"

// TODO: Разнести столы по файлам

class ITable {
public:
    virtual ~ITable() {}

    virtual void GameIteration() = 0;

    virtual void OneStep() = 0;

    virtual void AddPlayer(const IGambler &player);
    virtual void RemovePlayer(const IGambler &player);
};

class AbstractITable : public ITable {
public:
    AbstractITable()
        : whose_move_{0}, start_phase_{true} {}

protected:
    GameType table_type_;
    bool start_phase_; // This flag indiactes is game on the table just started,
                       // it made for convinient card distribution animation
                       // implementation
    size_t whose_move_; // index of the active player
    std::vector<std::shared_ptr<IGambler>> players_;
};

class PokerTable : public AbstractITable {
public:
private:
};

// Player by zero index is always a dealer
class BlackjackTable : public AbstractITable {
public:
    BlackjackTable() : AbstractITable() {}

    void GameIteration() override;

    void OneStep() override;

private:
    // returns first player in the order that still in game
    std::shared_ptr<IGambler> GetNextPlayer();

private:
    Deck deck_;
};

#endif // !CASINOOBSERVER_BRAINMANAGER_TABLE_H
