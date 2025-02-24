#pragma once

#include <game_object.hpp>
#include <iostream>
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
        std::cout << "Size: " << GetSize() << '\n';
        while (!render_queue_.empty() || current_render_event_.has_value()) {
            if (current_render_event_.has_value()) {
                std::cout << current_render_event_.value()["tag"].template get<std::string>() << ' '
                          << current_render_event_.value()["new_phase"].template get<std::string>()
                          << '\n';
                if (!TryHandle(current_render_event_.value()["event_type"],
                               current_render_event_.value())) {
                    break;
                }

                current_render_event_ = std::nullopt;
            }

            if (render_queue_.empty()) {
                break;
            }
            std::cout << "Change: " << "\n";
            current_render_event_ = render_queue_.pop();
        }
    }

    const IGameState* GetState() const { return state_; }

    TSQueue<T>& GetRenderQueue() { return render_queue_; }

    size_t GetSize() const { return render_queue_.size(); }

    bool IsEmpty() const { return render_queue_.empty(); }

private:
    IGameState* state_;
    TSQueue<T> render_queue_;
    std::optional<json> current_render_event_;
    std::unordered_map<std::string, event_handler> handlers_;
};
