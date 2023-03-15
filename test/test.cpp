#include <chrono>
#include <iostream>
#include <pl/thread_pool.hpp>
#include <pl/parallel.hpp>

#define CYCLES 100'000

class timer {
private:
    std::chrono::_V2::system_clock::time_point m_start;
    std::chrono::_V2::system_clock::time_point m_end;
public:
    void start() {
        m_start = std::chrono::system_clock::now();
    }

    void end() {
        m_end = std::chrono::system_clock::now();
    }

    void print(const std::string& _prefix = "") {
        printf("%s time - %f, done count\n", _prefix.c_str(), std::chrono::duration<float, std::chrono::milliseconds::period>(m_end - m_start).count());
    }
};

void thread_pool_test() {
    pl::thread_pool tp;

    size_t cycles = CYCLES;
    std::atomic<int> done;
    pl::thread_pool_task l_task;

    timer t;
    t.start();
    for (size_t i = 0; i < cycles; i++)
        l_task = tp.add_work([&]{ done++; });

    l_task.wait();
    t.end();
    t.print("Thread Pool Test");
}

void foreach_array_test() {
    timer t;
    std::atomic<int> final = 0;
    std::vector<int> v;
    v.resize(CYCLES, 2);

    t.start();

    pl::foreach<int>(v.data(), v.size(), [&](int &i) {
        final += i;
        return true;
    });

    t.end();
    t.print("Thread Pool Test");
    std::printf("final - %i - expected %i\n", final.load(), CYCLES * 2);
}

void for_array_test() {
    timer t;
    std::atomic<int> final = 0;
    std::vector<int> v;
    v.resize(CYCLES, 2);

    t.start();

    pl::_for(0, v.size(), [&](size_t& i) {
        final += v[i];
        return true;
    });

    t.end();
    t.print("Thread Pool Test");
    std::printf("final - %i - expected %i\n", final.load(), CYCLES * 2);
}

int main() {
    thread_pool_test();
    foreach_array_test();
    for_array_test();
    return 0;
}