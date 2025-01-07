// "Copyright [2024] Netsiaruk Uladzislau"
#ifndef CASINOOBSERVER_BRAINMANAGER_TABLE_H
#define CASINOOBSERVER_BRAINMANAGER_TABLE_H

#include <memory>
#include <vector>

#include "../Gamblers/gambler.h"
#include "SFML/System/Time.hpp"

class ITable {
public:
    virtual ~ITable() {}

    virtual void GameIteration() = 0;

    virtual void Dealing() = 0;
};

class AbstractTable : public ITable {
    sf::Time time_per_action_ = sf::seconds(1);

public:
    AbstractTable() : whose_move_{0} {}

    virtual void AddPlayer(IGambler& player) {
        players_.emplace_back(std::shared_ptr<IGambler>(&player));
    }

    virtual void RemovePlayer(const IGambler& player) {
        for (size_t ind = 0; ind < players_.size(); ind++) {
            if (&(*players_[ind]) == &player) {
                players_.erase(players_.begin() + ind);
                break;
            }
        }
    }

    bool IsGameFinished() {
        return whose_move_ >= players_.size();
    }

    void Update(sf::Time delta) {
        elapsed_ += delta;
        if (elapsed_ >= time_per_action_) {
            elapsed_ -= time_per_action_;
            GameIteration();
        }
    }

protected:
    size_t whose_move_; // index of the active player
    std::vector<std::shared_ptr<IGambler>> players_;
    sf::Time elapsed_ = sf::Time::Zero;
};

#endif // !CASINOOBSERVER_BRAINMANAGER_TABLE_H
