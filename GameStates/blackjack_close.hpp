// Copyright [2024] Nestsiaruk Uladzislau
#ifndef CASINOONSERVER_GAMESTATES_BLACKJACK_CLOSE_H
#define CASINOONSERVER_GAMESTATES_BLACKJACK_CLOSE_H

#include <memory>
#include <string>
#include <thread>

#include "SFML/Window/Event.hpp"
#include "game_state.h"
#include <../Tables/blackjack_table.hpp>
#include <drawer.hpp>
#include <game_object.hpp>
#include <json.hpp>

using json = nlohmann::json;

class BlackjackClose : public IGameState {
    enum class State { DEALING, PLAYING };
    const static std::string kBlackjackCloseGameObjects;

public:
    explicit BlackjackClose(StateManager* manager);

    void HandleEvent(const sf::Event& event) override;
    void Update(sf::Time delta) override;
    void Draw(StateManager* manager) override;

    ~BlackjackClose() override;

private:
    std::unique_ptr<ITable> table_;
    std::unique_ptr<IDrawer> drawer_;
    std::atomic<bool> run_game_;
    std::atomic<bool> stop_game_thread_;
    std::thread game_executor_;
    TSQueue<json> logs_;
    TSQueue<json> render_queue_;

    std::shared_ptr<GameObject> root_game_object_;

    void GameExecutor();
};

#endif // CASINOONSERVER_GAMESTATES_BLACKJACK_CLOSE_H
