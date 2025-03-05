#include <algorithm>
#include <array>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>

#include "SFML/Graphics/RenderWindow.hpp"
#include "game_object.hpp"
#include "game_object_manager.hpp"
#include "game_objects_loader.hpp"
#include "json.hpp"

GOManager::GOManager(std::string_view game_objects_path) : GOManager() {
    AddObjects(game_objects_path, 0);
}

void GOManager::AddObjects(std::string_view game_objects_path, size_t shared_priority) {
    std::vector<std::shared_ptr<GameObject>> objects = ParseGameObjects(game_objects_path);

    for (size_t object_ind = 0; object_ind < objects.size(); object_ind++) {
        AddObject(objects[object_ind], shared_priority);
    }
}

void GOManager::AddObjects(std::string_view game_objects_path, std::vector<size_t> priorities) {
    std::vector<std::shared_ptr<GameObject>> objects = ParseGameObjects(game_objects_path);
    if (objects.size() != priorities.size()) {
        throw std::invalid_argument("amount of root objects and defined priorities is different");
    }

    for (size_t object_ind = 0; object_ind < objects.size(); object_ind++) {
        AddObject(objects[object_ind], priorities[object_ind]);
    }
}

void GOManager::AddObject(std::shared_ptr<GameObject> game_object, size_t priority) {
    if (priority >= kMaxPriority) {
        throw std::invalid_argument("priority of the object exceeded max priority");
    }

    if (objects_.contains(game_object->GetTag())) {
        throw std::runtime_error("game object with such tag already exists. Problem tag: \"" +
                                 game_object->GetTag() + "\"");
    }

    objects_[game_object->GetTag()] = std::make_pair(game_object, priority);
    order_[priority].emplace_back(game_object->GetTag());
}

void GOManager::DrawAll(sf::RenderWindow* window) {
    for (int32_t priority = kMaxPriority - 1; priority >= 0; priority--) {
        std::for_each(order_[priority].begin(), order_[priority].end(),
                      [window, this](const std::string& object_tag) {
                          objects_[object_tag].first->Draw(window);
                      });
    }
}

std::shared_ptr<GameObject> GOManager::FindObjectByTag(const std::string& tag) const {
    for (auto& [root_tag, root_pair] : objects_) {
        std::shared_ptr<GameObject> result = root_pair.first->FindGameObjectByTag(tag);
        if (result != nullptr) {
            return result;
        }
    }

    return nullptr;
}

void GOManager::UpdateObjectPriority(const std::string& object_tag, size_t new_priority) {
    if (!objects_.contains(object_tag)) {
        throw std::runtime_error(
            "attempt to update of not registered or not root game object. Problem tag \"" +
            object_tag + "\"");
    }

    if (objects_[object_tag].second == new_priority) {
        return;
    }

    size_t old_priority = objects_[object_tag].second;
    order_[old_priority].erase(
        std::find(order_[old_priority].begin(), order_[old_priority].end(), object_tag));
    objects_[object_tag].second = new_priority;
    order_[new_priority].emplace_back(object_tag);
}

void GOManager::HandleEvent(IGameState* state, nlohmann::json& event_data, bool consider_cursor) {
    if (!CorrectEvent(event_data)) {
        return;
    }

    std::optional<std::string> handled_tag;
    for (int32_t priority = kMaxPriority - 1; priority >= 0 && !handled_tag.has_value();
         priority--) {
        for (const std::string& root_object_tag : order_[priority]) {
            handled_tag = objects_[root_object_tag].first->TriggerHandler(&StateManager::Instance(),
                                                                          state, event_data);
            if (handled_tag.has_value()) {
                handled_tag = root_object_tag;
                break;
            }
        }
    }

    if (consider_cursor && handled_tag.has_value()) {
        UpdateObjectPriority(handled_tag.value(), kMaxPriority - 1);
    }
}
