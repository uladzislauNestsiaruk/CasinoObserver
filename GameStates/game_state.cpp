#include "game_state.hpp"

std::shared_ptr<GameObject> AbstractGameState::FindGameObjectByTag(const std::string& tag) const {
    return root_game_object_->FindGameObjectByTag(tag);
}