#pragma once

#include <../game_states/game_state.hpp>
#include <../game_object/game_object.hpp>
#include <render_events_manager.hpp>

struct CommonGOEventHandlers {
    static DEFINE_GOHANDLER(ReturnButtonHandler);

    static DEFINE_GOHANDLER(BanButtonHandler);
    static DEFINE_GOHANDLER(SelectButtonHandler);
    static DEFINE_GOHANDLER(DealButtonHandler);

    static DEFINE_GOHANDLER(PlayerHandler);
};

struct CommonREMEventHandlers {
    static bool ChangePhaseHandler(RenderEventsManager<json>* render_manager, const json& data);
};
