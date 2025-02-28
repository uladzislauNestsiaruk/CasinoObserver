#pragma once

#include <../game_states/game_state.hpp>
#include <game_object.hpp>
#include <render_events_manager.hpp>

struct CommonGOEventHandlers {
    static void ReturnButtonHandler(StateManager* manager, IGameState* state, nlohmann::json& data);
    static void BanButtonHandler(StateManager* manager, IGameState* state, nlohmann::json& data);
    static void SelectButtonHandler(StateManager* manager, IGameState* state, nlohmann::json& data);
    static void DealButtonHandler(StateManager* manager, IGameState* state, nlohmann::json& data);
};

struct CommonREMEventHandlers {
    static bool ChangePhaseHandler(RenderEventsManager<json>* render_manager, const json& data);
};
