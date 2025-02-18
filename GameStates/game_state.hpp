#pragma once

#include "SFML/Window/Event.hpp"

#include <game_objects_loader.hpp>
#include <json.hpp>
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
        : render_events_manager_(this), root_game_object_(ParseGameObjects(game_objects_path)) {}

    virtual ~AbstractGameState() {}

    std::shared_ptr<GameObject> FindGameObjectByTag(const std::string& tag) const;

protected:
    std::shared_ptr<GameObject> root_game_object_;
    RenderEventsManager<json> render_events_manager_;
};
