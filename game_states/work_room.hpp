#pragma once

#include <string>

#include <../game_object/game_object.hpp>
#include <json.hpp>
#include <thread_safe_queue.hpp>

#include "SFML/System/Time.hpp"
#include "game_state.hpp"
#include "state_manager.hpp"
#include "table_state.hpp"

using json = nlohmann::json;

class WorkRoomState : public AbstractGameState {
    const static std::string kWorkRoomGameObjects;

    template <typename TableType>
        requires std::is_base_of_v<ITable, TableType>
    static std::unique_ptr<ITable> CreateTable(TSQueue<json>& logs, TSQueue<json>& render_queue) {
        return std::make_unique<TableType>(logs, render_queue);
    }

public:
    explicit WorkRoomState(StateManager* manager);

    void HandleEvent(const sf::Event& event) override;
    void Update(sf::Time delta) override;
    void Draw(StateManager* manager) override;

    ~WorkRoomState() override {}

private:
    std::vector<std::shared_ptr<TableState>> tables_;

    mutable size_t total_tables = 12;

    friend DEFINE_GOHANDLER(BlackjackScreenHandler);
    friend DEFINE_GOHANDLER(PokerScreenHandler);
};
