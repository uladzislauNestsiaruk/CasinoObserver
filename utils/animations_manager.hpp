#pragma once

#include <unordered_set>

class AnimationsManager {
public:
    AnimationsManager() : animation_id_(0), active_animations_() {}

    uint64_t AddNGetAnimationId() {
        uint64_t id = animation_id_.fetch_add(1);
        active_animations_.insert(id);
        return id;
    }

    uint64_t FinishAnimationId(uint64_t id) { return active_animations_.erase(id); }

    bool CheckActiveAnimationId(uint64_t id) { return active_animations_.contains(id); }

private:
    std::atomic<uint64_t> animation_id_;
    std::unordered_set<uint64_t> active_animations_;
};