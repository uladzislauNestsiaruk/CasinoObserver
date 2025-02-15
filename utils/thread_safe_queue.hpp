#pragma once

#include <atomic>
#include <new>
#include <optional>

namespace {
template <typename T>
struct Node {
    Node() : nxt(nullptr), data() {}

    explicit Node(const T& val) : nxt(nullptr), data(val) {}

    std::atomic<Node*> nxt;
    T data;
};
}; // namespace

template <typename T> class TSQueue {
    using Node = Node<T>;

public:
    TSQueue() : size_(0) {
        Node* dummy = new Node();
        tail_ = dummy;
        head_ = dummy;
    }

    TSQueue(const TSQueue& other_tsq) = delete;
    TSQueue& operator=(const TSQueue& other_tsq) = delete;

    TSQueue(TSQueue&& other_tsq) = delete;
    TSQueue&& operator=(TSQueue& other_tsq) = delete;

    ~TSQueue() {
        Node* expected = head_.load();
        while (expected != nullptr) {
            Node* copy = expected;
            if (head_.compare_exchange_weak(expected, expected->nxt)) {
                delete copy;
            }

            expected = head_.load();
        }
    }

    std::optional<T> pop() {
        while (true) {
            Node* expected_tail = tail_.load();
            Node* expected_head = head_.load();
            Node* expected_head_nxt = expected_head->nxt.load();
            if (expected_tail == expected_head) {
                if (expected_head->nxt.load() == nullptr) {
                    return std::nullopt;
                }

                tail_.compare_exchange_weak(expected_tail, expected_head_nxt);
            } else if (head_.compare_exchange_weak(expected_head, expected_head_nxt)) {
                size_.fetch_sub(1);
                return std::optional<T>(expected_head_nxt->data);
            }
        }
    }

    
    void push(const T& value) {
        Node* new_element = new Node(value);
        while (true) {
            Node* expected_nxt = nullptr;
            Node* expected_tail = tail_.load();

            if (tail_.load()->nxt.compare_exchange_weak(expected_nxt, new_element)) {
                size_.fetch_add(1);
                tail_.compare_exchange_weak(expected_tail, new_element);
                return;
            } else {
                tail_.compare_exchange_weak(expected_tail, expected_tail->nxt.load());
            }
        }
    }

    size_t size() const { return size_.load(); }

    bool empty() const { return !size_.load(); }

private:
    std::atomic<Node*> tail_;
    std::atomic<Node*> head_;
    std::atomic<size_t> size_;
};
