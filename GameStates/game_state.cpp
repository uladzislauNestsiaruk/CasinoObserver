#include "game_state.hpp"

std::shared_ptr<GameObject> AbstractGameState::FindGameObjectByTag(const std::string& tag) const {
    return objects_manager_.FindObjectByTag(tag);
}
