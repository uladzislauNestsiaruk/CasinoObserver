#pragma once

#include <mutex>
#include <queue>

// Thread safe queue
// It is slow and basic implementation
// TODO: lock-free queue
template <typename T>
class TSQueue {
public:
    TSQueue() = default;

    TSQueue(const TSQueue& other_tsq) = delete;
    TSQueue& operator=(const TSQueue& other_tsq) = delete;

    TSQueue(TSQueue&& other_tsq) = delete;
    TSQueue&& operator=(TSQueue& other_tsq) = delete;

    ~TSQueue() = default;

    std::optional<T> pop() {
        std::lock_guard<std::mutex> lg{mtx_};
        if (data_.empty()) {
            return std::nullopt;
        }

        T elem = data_.front();
        data_.pop();
        return elem;
    }

    void push(const T& value) {
        std::lock_guard<std::mutex> lg{mtx_};
        data_.push(value);
    }

    size_t size() const {
        std::lock_guard<std::mutex> lg{mtx_};
        return data_.size();
    }

    bool empty() const {
        std::lock_guard<std::mutex> lg{mtx_};
        return data_.empty();
    }

private:
    std::queue<T> data_;
    mutable std::mutex mtx_;
};