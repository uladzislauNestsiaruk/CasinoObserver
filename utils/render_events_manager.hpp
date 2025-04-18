#pragma once

#include <../game_object/game_object.hpp>
#include <json.hpp>
#include <optional>
#include <string>
#include <thread_safe_queue.hpp>

using nlohmann::json;

template <typename T> class RenderEventsManager {
public:
    using event_handler = bool (*)(RenderEventsManager* render_manager, const json& data);

    RenderEventsManager(IGameState* state) : state_(state) {}

    void AddHandler(const std::string& event_type, event_handler handler) {
        handlers_[event_type] = handler;
    }

    bool TryHandle(const std::string& event_type, const json& data) {
        return handlers_[event_type](this, data);
    }

    void HandleEvent() {
        while (!render_queue_.empty() || current_render_event_.has_value()) {
            if (current_render_event_.has_value()) {
                if (!TryHandle(current_render_event_.value()["event"]["type"],
                               current_render_event_.value())) {
                    break;
                }

                current_render_event_ = std::nullopt;
            }

            if (render_queue_.empty()) {
                break;
            }
            current_render_event_ = render_queue_.pop();
        }
    }

    const IGameState* GetState() const { return state_; }

    TSQueue<T>& GetRenderQueue() { return render_queue_; }

    size_t GetSize() const { return render_queue_.size(); }

    bool IsEmpty() const { return render_queue_.empty(); }

    void Clear() {
        current_render_event_ = std::nullopt;
        while (!render_queue_.empty()) {
            render_queue_.pop();
        }
    }

private:
    IGameState* state_;
    TSQueue<T> render_queue_;
    std::optional<json> current_render_event_;
    std::unordered_map<std::string, event_handler> handlers_;
};
