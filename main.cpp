#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <future>
#include <algorithm>
#include <functional>
#include <numeric>
#include "barrier.h"

int global = 0;

using std::thread;
using std::vector;
using namespace std::chrono_literals;

inline uint64_t read_rdtsc(void)
{
    uint64_t d;
    __asm__ __volatile__ ("rdtsc" : "=A" (d));
    return d;
}

void foo(barrier &b, uint64_t &t, int i) {
    b.wait();
    t = read_rdtsc();
}

class scoped_thread {
private:
    std::thread t{};

public:
    scoped_thread() = delete;

    explicit scoped_thread(std::thread _t)
            : t{std::move(_t)} {
        if (!t.joinable())
            throw std::logic_error("Thread is not joinable");
    }

    template<class ...Args>
    explicit scoped_thread(Args &&... args) noexcept
            : t{std::forward<Args>(args)...} {}

    scoped_thread(const scoped_thread &) = delete;

    scoped_thread(scoped_thread &&other) noexcept
            : t{std::move(other.t)} {
    }

    scoped_thread &operator=(const scoped_thread &) = delete;

    scoped_thread &operator=(scoped_thread &&) noexcept = default;

    ~scoped_thread() {
        if (t.joinable())
            t.join();
    }
};

int main() {
    int tries = 0;
    std::vector<uint64_t> diffs;
    for (int i = 0; i < 100000; ++i) {
        barrier b{8};
        std::vector<uint64_t> ts(8);
        {
            global = 0;
            vector<scoped_thread> threads;
            threads.emplace_back(foo, std::ref(b), std::ref(ts[0]), 1);
            threads.emplace_back(foo, std::ref(b), std::ref(ts[1]), 2);
            threads.emplace_back(foo, std::ref(b), std::ref(ts[2]), 3);
            threads.emplace_back(foo, std::ref(b), std::ref(ts[3]), 4);
            threads.emplace_back(foo, std::ref(b), std::ref(ts[4]), 5);
            threads.emplace_back(foo, std::ref(b), std::ref(ts[5]), 6);
            threads.emplace_back(foo, std::ref(b), std::ref(ts[6]), 7);
            threads.emplace_back(foo, std::ref(b), std::ref(ts[7]), 8);
        }

        std::sort(ts.begin(), ts.end());
        diffs.push_back(*ts.rbegin() - *ts.begin());
        ++tries;
    }

    std::cout << tries << " " << std::accumulate(diffs.begin(), diffs.end(), uint64_t{0})/diffs.size() << std::endl;
    return 0;
}