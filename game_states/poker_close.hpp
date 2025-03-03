#pragma once

#include <string>
#include <thread>
#include <unordered_set>

#include "../tables/poker_table.hpp"
#include "game_state.hpp"
#include <game_object.hpp>
#include <json.hpp>
#include <render_events_manager.hpp>
#include <thread_safe_queue.hpp>

using json = nlohmann::json;

class PokerClose : public AbstractGameState {
    static const std::string kPokerCloseGameObjects;

public:
    explicit PokerClose(StateManager* manager);

    void HandleEvent(const sf::Event& event) override;
    void Update(sf::Time delta) override;
    void Draw(StateManager* manager) override;

    ~PokerClose() override;

    void SetIsSelectPressed(bool value) { is_select_pressed_ = value; }

    bool GetIsSelectPressed() const { return is_select_pressed_; }

    size_t SizeSelectPlayer() const { return selected_players_.size(); }

    bool ContainsSelectPlayer(const std::string& player_tag) const {
        return selected_players_.contains(player_tag);
    }

    void AddSelectPlayer(const std::string& player_tag) { selected_players_.insert(player_tag); }

    void EraseSelectPlayer(const std::string& player_tag) { selected_players_.erase(player_tag); }

    void BanPlayers();

private:
    void GameExecutor();

private:
    bool is_select_pressed_ = false;
    std::unordered_set<std::string> selected_players_;

    std::atomic<bool> stop_game_thread_;
    std::atomic<bool> run_game_;
    std::thread game_exec_thr_;

    std::unique_ptr<ITable> table_;

    TSQueue<json> logs_;
};
