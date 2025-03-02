#pragma once

#include <array>
#include <memory>
#include <string_view>
#include <vector>

#include "SFML/Graphics/RenderWindow.hpp"
#include "game_object.hpp"
#include "game_objects_loader.hpp"

class GEManager {
    static constexpr size_t kMaxPriority = 32;

public:
    GEManager() : objects_(), priority_() {}

    explicit GEManager(std::string_view game_object_path)
        : objects_(1, ParseGameObjects(game_object_path)), priority_(1, 1) {}

    void AddObject(std::string_view game_object_path, size_t priority = 1);

    void AddObject(std::shared_ptr<GameObject> game_object, size_t priority = 1);

    std::shared_ptr<GameObject> FindObjectByTag(const std::string& tag) const;

    void HandleEvent(IGameState* state, nlohmann::json& event_data);

    void DrawAll(sf::RenderWindow* window);

    ~GEManager() {}

private:
    std::vector<std::shared_ptr<GameObject>> objects_;
    std::vector<size_t> priority_;

    std::array<std::vector<size_t>, kMaxPriority> GetOrder();
    void UpdateObjectPriority(size_t object_ind, size_t new_priority);
};
