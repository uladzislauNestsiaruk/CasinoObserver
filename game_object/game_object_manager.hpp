#pragma once

#include <array>
#include <iostream>
#include <memory>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "SFML/Graphics/RenderWindow.hpp"
#include "game_object.hpp"

class GOManager {
    static constexpr size_t kMaxPriority = 16;

public:
    GOManager() : objects_(), order_() {}
    explicit GOManager(std::string_view game_object_path);

    GOManager(const GOManager& manager) = delete;
    GOManager& operator=(const GOManager& rhs) = delete;

    void AddObjects(std::string_view game_objects_path, size_t shared_priority = 1);
    void AddObjects(std::string_view game_objects_path, std::vector<size_t> priorities);
    void AddObject(std::shared_ptr<GameObject> game_object, size_t priority = 1);

    std::shared_ptr<GameObject> FindObjectByTag(const std::string& tag) const;

    void UpdateObjectPriority(const std::string& object_tag, size_t new_priority);

    void HandleEvent(IGameState* state, nlohmann::json& event_data, bool consider_cursor = true);

    void DrawAll(sf::RenderWindow* window);

private:
    std::unordered_map<std::string, std::pair<std::shared_ptr<GameObject>, size_t>> objects_;
    std::array<std::vector<std::string>, kMaxPriority> order_;
};
