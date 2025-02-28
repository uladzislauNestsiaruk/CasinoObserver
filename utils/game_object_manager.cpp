#include <array>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string_view>

#include "SFML/Graphics/RenderWindow.hpp"
#include "game_object.hpp"
#include "game_object_manager.hpp"
#include "json.hpp"

std::array<std::vector<size_t>, GEManager::kMaxPriority> GEManager::GetOrder() {
    std::array<std::vector<size_t>, kMaxPriority> order;
    for (size_t ind = 0; ind < objects_.size(); ind++) {
        order[priority_[ind]].emplace_back(ind);
    }
    return order;
}

void GEManager::AddObject(std::string_view game_object_path, size_t priority) {
    if (priority >= kMaxPriority) {
        throw std::invalid_argument("priority of the object exceeded max priority");
    }

    objects_.emplace_back(ParseGameObjects(game_object_path));
    priority_.emplace_back(priority);
}

void GEManager::AddObject(std::shared_ptr<GameObject> game_object, size_t priority) {
    if (priority >= kMaxPriority) {
        throw std::invalid_argument("priority of the object exceeded max priority");
    }
    objects_.emplace_back(game_object);
    priority_.emplace_back(priority);
}

void GEManager::DrawAll(sf::RenderWindow* window) {
    auto draw_order = GetOrder();
    for (int32_t priority = kMaxPriority - 1; priority >= 0; priority--) {
        std::for_each(draw_order[priority].begin(), draw_order[priority].end(),
                      [window, this](size_t ind) { objects_[ind]->Draw(window); });
    }
}

std::shared_ptr<GameObject> GEManager::FindObjectByTag(const std::string& tag) const {
    for (auto object : objects_) {
        std::shared_ptr<GameObject> result = object->FindGameObjectByTag(tag);
        if (result != nullptr) {
            return result;
        }
    }

    return nullptr;
}

void GEManager::HandleEvent(IGameState* state, nlohmann::json& event_data) {
    auto handle_order = GetOrder();
    for (int32_t priority = kMaxPriority - 1; priority >= 0; priority--) {
        std::optional<std::string> handled;
        for (size_t ind : handle_order[priority]) {
            handled = objects_[ind]->TriggerHandler(&StateManager::Instance(), state, event_data);
            if (handled.has_value()) {
                break;
            }
        }

        if (handled.has_value()) {
            break;
        }
    }
}
