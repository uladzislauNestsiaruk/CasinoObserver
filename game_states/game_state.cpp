#include "game_state.hpp"
#include <memory>

std::shared_ptr<GameObject> AbstractGameState::FindGameObjectByTag(const std::string& tag) const {
    return objects_manager_.FindObjectByTag(tag);
}

void AbstractGameState::AddIndependentObject(std::shared_ptr<GameObject> object) noexcept {
    objects_manager_.AddObject(object);
}
