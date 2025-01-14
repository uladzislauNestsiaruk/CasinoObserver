// "Copyright [2024] Netsiaruk Uladzislau"
#ifndef CASINOOBSERVER_BRAINMANAGER_TABLE_H
#define CASINOOBSERVER_BRAINMANAGER_TABLE_H

#include <climits>
#include <memory>
#include <shared_mutex>
#include <vector>

#include "../Gamblers/gambler.h"
#include "SFML/System/Time.hpp"

class ITable {
public:
    virtual ~ITable() {}

    virtual void AddPlayer(std::shared_ptr<IGambler> player) = 0;

    virtual void RemovePlayer(const IGambler& player) = 0;

    virtual void GameIteration() = 0;

    virtual void Dealing() = 0;

    virtual bool IsGameFinished() const = 0;
};

class AbstractTable : public ITable {
    sf::Time time_per_action_ = sf::seconds(1);

public:
    AbstractTable() : whose_move_{0} {}

    // Ctor for dealer specialization
    explicit AbstractTable(GameType table_type)
        : whose_move_{0}, players_(1, std::make_shared<HumbleGambler>(0, table_type, 0, INT_MAX)) {}

    void AddPlayer(std::shared_ptr<IGambler> player) override { players_.emplace_back(player); }

    void RemovePlayer(const IGambler& player) override {
        for (size_t ind = 0; ind < players_.size(); ind++) {
            if (&(*players_[ind]) == &player) {
                players_.erase(players_.begin() + ind);
                break;
            }
        }
    }

    virtual void Clean() = 0;

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
