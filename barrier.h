//
// Created by adamo on 2018-03-25.
//

#ifndef THREAD_WORKSHOP_BARRIER_H
#define THREAD_WORKSHOP_BARRIER_H

#include <atomic>
#include <cstddef>
#include <shared_mutex>
#include <condition_variable>

#include <chrono>

using namespace std::chrono_literals;

class barrier {
private:
    std::atomic<std::size_t> count{0};
    const std::size_t spaces;
    int generation{0};
    std::shared_mutex mutex;
    std::condition_variable_any conditionVariable;

public:
    explicit barrier(std::size_t count) noexcept: spaces(count) {}

    barrier() = delete;

    barrier(const barrier &) = delete;

    barrier(barrier &&) = default;

    barrier &operator=(const barrier &) = delete;

    barrier &operator=(barrier &&) = default;

    void wait() {
        const int my_generation = generation;
        if (count.fetch_add(1) + 1 >= spaces) {
            {
                std::lock_guard lock{mutex};
                ++generation;
            }
            conditionVariable.notify_all();
        } else {
            std::shared_lock lock{mutex};
            conditionVariable.wait(mutex, [&]{return my_generation != generation;});
        }
    }
};

class barrier2 {
private:
    mutable std::atomic<std::size_t> count{0};
    const std::size_t spaces;
    mutable std::promise<void> p{};
    const std::shared_future<void> f{p.get_future()};

public:
    explicit barrier2(std::size_t count) noexcept: spaces(count) {}

    barrier2() = delete;

    barrier2(const barrier2 &) = delete;

    barrier2(barrier2 &&) = default;

    barrier2 &operator=(const barrier2 &) = delete;

    barrier2 &operator=(barrier2 &&) = default;

    void wait() const {
        const auto my_f = f;
        if (count.fetch_add(1) + 1 >= spaces) {
            p.set_value();
        } else {
            my_f.get();
        }
    }
};

#endif //THREAD_WORKSHOP_BARRIER_H
