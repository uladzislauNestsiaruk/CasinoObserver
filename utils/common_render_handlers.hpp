#pragma once

#include <memory>

#include <../GameStates/game_state.hpp>
#include <game_object.hpp>
#include <render_events_manager.hpp>

struct CommonGOEventHandlers {
    static DEFINE_GOHANDLER(ReturnButtonHandler);
};

struct CommonREMEventHandlers {
    static bool ChangePhaseHandler(RenderEventsManager<json>* render_manager, const json& data);
};
