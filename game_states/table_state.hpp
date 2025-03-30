#pragma once

#include <animations_manager.hpp>
#include <memory>
#include <thread>
#include <unordered_set>

#include <../tables/table.hpp>
#include <common_render_handlers.hpp>

#include "game_state.hpp"
#include "state_manager.hpp"

class TableState : public AbstractGameState {
    using table_creater_t = std::unique_ptr<ITable> (*)(TSQueue<json>& logs,
                                                        TSQueue<json>& render_queue,
                                                        AnimationsManager& animations_manager);

public:
    TableState() = delete;
    TableState(const std::string& objects_path, StateManager* manager,
               table_creater_t table_creater);

    void HandleEvent(const sf::Event& event) override;
    void Update(sf::Time delta) override;
    void Draw(StateManager* manager) override;

    void SetIsSelectPressed(bool value) { is_select_pressed_ = value; }
    bool GetIsSelectPressed() const { return is_select_pressed_; }

    size_t SizeSelectPlayer() const { return selected_players_.size(); }

    bool ContainsSelectPlayer(const std::string& player_tag) const {
        return selected_players_.contains(player_tag);
    }

    void AddSelectPlayer(const std::string& player_tag) { selected_players_.insert(player_tag); }
    void EraseSelectPlayer(const std::string& player_tag) { selected_players_.erase(player_tag); }

    void BanPlayers();

    void DealPlayers(StateManager* manager);

    ~TableState() override;

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
