#include <algorithm>
#include <array>
#include <iostream>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>

#include "SFML/Graphics/RenderWindow.hpp"
#include "game_object.hpp"
#include "game_object_manager.hpp"
#include "json.hpp"

std::array<std::vector<size_t>, GOManager::kMaxPriority> GOManager::GetOrder() {
    std::array<std::vector<size_t>, kMaxPriority> order;
    for (size_t ind = 0; ind < objects_.size(); ind++) {
        order[priority_[ind]].emplace_back(ind);
    }
    return order;
}

void GOManager::AddObject(std::string_view game_object_path, size_t priority) {
    if (priority >= kMaxPriority) {
        throw std::invalid_argument("priority of the object exceeded max priority");
    }

    std::vector<std::shared_ptr<GameObject>> objects = ParseGameObjects(game_object_path);
    if (objects.size() > 1) {
        throw std::logic_error("attempt to add multiple independent objects");
    }

    objects_.emplace_back(objects[0]);
    priority_.emplace_back(priority);
}

void GOManager::AddObject(std::shared_ptr<GameObject> game_object, size_t priority) {
    if (priority >= kMaxPriority) {
        throw std::invalid_argument("priority of the object exceeded max priority");
    }
    objects_.emplace_back(game_object);
    priority_.emplace_back(priority);
}

void GOManager::DrawAll(sf::RenderWindow* window) {
    auto draw_order = GetOrder();
    for (int32_t priority = kMaxPriority - 1; priority >= 0; priority--) {
        std::for_each(draw_order[priority].begin(), draw_order[priority].end(),
                      [window, this](size_t ind) { objects_[ind]->Draw(window); });
    }
}

std::shared_ptr<GameObject> GOManager::FindObjectByTag(const std::string& tag) const {
    for (auto object : objects_) {
        std::shared_ptr<GameObject> result = object->FindGameObjectByTag(tag);
        if (result != nullptr) {
            return result;
        }
    }

    return nullptr;
}

void GOManager::UpdateObjectPriority(size_t object_ind, size_t new_priority) {
    priority_[object_ind] = new_priority;
    auto handle_order = GetOrder();
    size_t min_priority = 0;

    for (size_t priority = 0; priority < kMaxPriority; priority++) {
        if (handle_order[priority].empty()) {
            continue;
        }
        if (priority == min_priority) {
            ++min_priority;
            continue;
        }
        std::for_each(handle_order[priority].begin(), handle_order[priority].end(),
                      [this, min_priority](size_t ind) { priority_[ind] = min_priority; });
        ++min_priority;
    }
}

void GOManager::HandleEvent(IGameState* state, nlohmann::json& event_data) {
    if (!CorrectEvent(event_data)) {
        return;
    }
    auto handle_order = GetOrder();
    std::optional<std::string> handled_tag;
    size_t handled_ind;
    for (int32_t priority = kMaxPriority - 1; priority >= 0 && !handled_tag.has_value();
         priority--) {
        for (size_t ind : handle_order[priority]) {
            handled_tag =
                objects_[ind]->TriggerHandler(&StateManager::Instance(), state, event_data);
            if (handled_tag.has_value()) {
                handled_ind = ind;
                std::cout << "after finding trigger\n";
                break;
            }
        }
    }

    if (handled_tag.has_value()) {
        UpdateObjectPriority(handled_ind, kMaxPriority - 1);
    }
}
