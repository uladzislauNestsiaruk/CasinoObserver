#pragma once

#include "SFML/Window/Event.hpp"

#include <../game_object/game_object_manager.hpp>
#include <../game_object/game_objects_loader.hpp>
#include <animations_manager.hpp>
#include <json.hpp>
#include <memory>
#include <render_events_manager.hpp>

using nlohmann::json;

class GameObject;

class StateManager;

class IGameState {
public:
    IGameState() = default;

    virtual void HandleEvent(const sf::Event& event) = 0;
    virtual void Update(sf::Time delta) = 0;
    virtual void Draw(StateManager* manager) = 0;

    void ChangeState();

    virtual ~IGameState() {}
};

class AbstractGameState : public IGameState {
public:
    AbstractGameState(std::string_view game_objects_path)
        : animations_mananger_(), objects_manager_(game_objects_path, animations_mananger_),
          render_events_manager_(this) {}

    virtual ~AbstractGameState() {}

    void AddIndependentObject(std::shared_ptr<GameObject> object) noexcept;

    std::shared_ptr<GameObject> FindGameObjectByTag(const std::string& tag) const;

    AnimationsManager& GetAnimationsManager() { return animations_mananger_; }

protected:
    AnimationsManager animations_mananger_;
    GOManager objects_manager_;
    RenderEventsManager<json> render_events_manager_;
};
