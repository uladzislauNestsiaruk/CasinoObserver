#pragma once

#include <memory>
#include <vector>

#include <json.hpp>
#include <thread_safe_queue.hpp>

#include "../gamblers/gambler.hpp"
#include "SFML/System/Time.hpp"

using nlohmann::json;

class ITable {
public:
    virtual ~ITable() {}

    virtual const std::vector<std::shared_ptr<IGambler>>& GetPlayers() const = 0;

    virtual void AddPlayer(std::shared_ptr<IGambler> player) = 0;

    virtual void RemovePlayer(size_t ind) = 0;

    virtual void GameIteration() = 0;

    virtual void Dealing() = 0;

    virtual bool IsGameFinished() const = 0;
};

class AbstractTable : public ITable {
    sf::Time time_per_action_ = sf::seconds(1);

public:
    AbstractTable(TSQueue<json>& render_queue) : whose_move_{0}, render_queue_(render_queue) {}

    explicit AbstractTable(GameType table_type, TSQueue<json>& render_queue)
        : whose_move_{0}, render_queue_(render_queue) {}

    const std::vector<std::shared_ptr<IGambler>>& GetPlayers() const override { return players_; }

    void AddPlayer(std::shared_ptr<IGambler> player) override;

    void RemovePlayer(size_t ind) override;

    virtual void Clean() = 0;

    void Update(sf::Time delta);

protected:
    size_t whose_move_; // index of the active player
    std::vector<std::shared_ptr<IGambler>> players_;
    std::array<bool, 6> occupied_places_;
    sf::Time elapsed_ = sf::Time::Zero;
    TSQueue<json>& render_queue_;
};
