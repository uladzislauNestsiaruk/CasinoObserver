#pragma once

#include <game_object.hpp>
#include <json.hpp>
#include <string>
#include <thread_safe_queue.hpp>

using nlohmann::json;

template <typename T>
class RenderEventsManager {
public:
    using event_handler = bool(*)(RenderEventsManager* render_manager, const json& data);

    RenderEventsManager(IGameState* state) : state_(state) {
    }

    void AddHandler(const std::string& event_type, event_handler handler) {
        handlers_[event_type] = handler;
    }

    bool TryHandle(const std::string& event_type, const json& data) {
        return handlers_[event_type](this, data);
    }

    void HandleEvent() {
        static std::optional<json> render_event = std::nullopt;
        if (render_event != std::nullopt && TryHandle(render_event.value()["event_type"], render_event.value())) {
            render_event = std::nullopt;
        } else {
            while (!render_queue_.empty()) {
                if (render_event != std::nullopt) {
                    break;
                }
                render_event = render_queue_.pop();
            }
        }
    }

    const IGameState* GetState() const {
        return state_;
    }

    TSQueue<T>& GetRenderQueue() {
        return render_queue_;
    }

    size_t GetSize() const {
        return render_queue_.size();
    }

    bool IsEmpty() const {
        return render_queue_.empty();
    }

private:
    IGameState* state_;
    TSQueue<T> render_queue_;
    std::unordered_map<std::string, event_handler> handlers_;
};