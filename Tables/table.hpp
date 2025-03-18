#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include <deck.hpp>
#include <json.hpp>
#include <thread_safe_queue.hpp>

#include "../gamblers/gambler.hpp"
#include "SFML/System/Time.hpp"
#include "constants.hpp"

using nlohmann::json;

class ITable {
public:
    virtual ~ITable() {}

    virtual const std::vector<std::shared_ptr<IGambler>>& GetPlayers() const = 0;

    virtual void AddPlayer(std::shared_ptr<IGambler> player) = 0;

    virtual void RemovePlayer(size_t ind) = 0;

    virtual void RemovePlayer(const std::string& person_tag) = 0;

    virtual void GameIteration() = 0;

    virtual void Dealing() = 0;

    virtual bool IsGameFinished() const = 0;

    virtual bool GetWasActionPerformed() const = 0;

    virtual void SetWasActionPerformed(bool value) = 0;

    virtual void ClearRollback() = 0;

    virtual void RollbackGame() = 0;

    virtual void SetIsActiveGame(bool value) = 0;

    virtual void AddRenderEvent(const json&) = 0;

    virtual GameType GetGameType() = 0;
};

class AbstractTable : public ITable {
    sf::Time time_per_action_ = sf::seconds(1);

public:
    explicit AbstractTable(GameType game_type, TSQueue<json>& render_queue,
                           bool without_jokers = true)
        : game_type_(game_type), deck_(without_jokers), whose_move_{0},
          render_queue_(render_queue) {}

    const std::vector<std::shared_ptr<IGambler>>& GetPlayers() const override { return players_; }

    void AddPlayer(std::shared_ptr<IGambler> player) override;

    void RemovePlayer(size_t ind) override;

    void RemovePlayer(const std::string& person_tag) override;

    void GenPlayers(uint8_t num_players, uint8_t max_places, GameType type,
                    uint16_t left_money_bound = 100, uint16_t right_money_bound = 500);

    virtual void Clean() = 0;

    void Update(sf::Time delta);

    bool GetWasActionPerformed() const override { return was_action_performed_.load(); }

    void SetWasActionPerformed(bool value) override { was_action_performed_.store(value); }

    void AddRenderEvent(const json& render_event) override { render_queue_.push(render_event); }

    bool IsPlaceOccupied(size_t place_id) { return occupied_places_[place_id]; }

    GameType GetGameType() override { return game_type_; }

protected:
    GameType game_type_;
    std::atomic<bool> was_action_performed_ = false;
    Deck deck_;
    size_t whose_move_; // index of the active player
    std::vector<std::shared_ptr<IGambler>> players_;
    std::array<bool, 6> occupied_places_;
    sf::Time elapsed_ = sf::Time::Zero;
    TSQueue<json>& render_queue_;
};
