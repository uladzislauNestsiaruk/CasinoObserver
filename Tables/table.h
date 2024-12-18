// "Copyright [2024] Netsiaruk Uladzislau"
#ifndef CASINOOBSERVER_BRAINMANAGER_TABLE_H
#define CASINOOBSERVER_BRAINMANAGER_TABLE_H

#include <memory>
#include <vector>

#include "../deck.h"
#include "constants.h"
#include "../Gamblers/gambler.h"

class ITable {
public:
    virtual ~ITable() {}

    virtual void GameIteration() = 0;

    virtual void Dealing() = 0;

    virtual void AddPlayer(const IGambler& player);
    virtual void RemovePlayer(const IGambler& player);
};

class AbstractITable : public ITable {
public:
    AbstractITable() : whose_move_{0} {}

protected:
    size_t whose_move_; // index of the active player
    std::vector<std::shared_ptr<IGambler>> players_;
};

#endif // !CASINOOBSERVER_BRAINMANAGER_TABLE_H
