#include <iostream>
#include <thread>
#include <vector>

int global = 0;
int tries = 0;

using std::thread;
using std::vector;

void foo(int i)
{
    global += i;
}

int main() {
    do {
        global = 0;
        vector<thread> threads;
        threads.emplace_back(foo, 1);
        threads.emplace_back(foo, 2);
        threads.emplace_back(foo, 3);
        threads.emplace_back(foo, 4);
        threads.emplace_back(foo, 5);
        threads.emplace_back(foo, 6);
        threads.emplace_back(foo, 7);
        threads.emplace_back(foo, 8);

        ++tries;

        for (auto &t: threads)
            t.join();

    } while (global == 36);
    std::cout << tries << " " << global << std::endl;
    return 0;
}