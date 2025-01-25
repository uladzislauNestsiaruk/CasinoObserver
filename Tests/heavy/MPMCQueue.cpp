#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <algorithm>
#include <cstddef>
#include <ranges>
#include <thread>

#include "thread_safe_queue.hpp"

TEST(MPMCQueue, DataLoss) {
    TSQueue<size_t> queue;
    size_t threads_limit = std::thread::hardware_concurrency() << 1;
    const size_t count = 1e6;
    std::vector<std::atomic<size_t>> cnt(count);

    auto IntegerSpamer = [&queue](size_t count) {
        auto push = [&](size_t val) { queue.push(val); };
        std::ranges::for_each(std::ranges::iota_view(0ull, count), push);
    };

    auto IntegerReciever = [&queue, &cnt]() {
        while (!queue.empty()) {
            std::optional<size_t> opt = queue.pop();
            if (opt.has_value()) {
                cnt[opt.value()].fetch_add(1);
            }
        }
    };

    std::vector<std::thread> threads(threads_limit);
    std::generate(threads.begin(), threads.end(),
                  [&]() { return std::thread(IntegerSpamer, count); });
    std::for_each(threads.begin(), threads.end(), [](std::thread& thr) { thr.join(); });

    ASSERT_EQ(count * threads_limit, queue.size());

    std::generate(threads.begin(), threads.end(), [&]() { return std::thread(IntegerReciever); });
    std::for_each(threads.begin(), threads.end(), [](std::thread& thr) { thr.join(); });

    std::for_each(cnt.begin(), cnt.end(), [&](size_t appear) { ASSERT_EQ(appear, threads_limit); });
}
