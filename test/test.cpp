#include <chrono>
#include <iostream>
#include "pl/parallel.hpp"

using namespace pl;
using namespace std::chrono;

auto start = high_resolution_clock::now();

inline void start_timer() { start = high_resolution_clock::now(); }

inline long long stop_timer(const char* prefix) {
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);
    std::cout << prefix << duration.count() << "ms" << std::endl;
    return duration.count();
}

std::vector<int> values;

void reset() {
    parallel::_for(0, values.size(), [&](int i) {
        values[i] = i;
        return true;
    });
}

inline int calculation(int value) {
    return sqrt(value) * pow(value, 2);
}

void speed_test() {
    ///Test parallel for
    start_timer();

    parallel::_for(0, values.size(), [&](int i) {
        values[i] = calculation(i);
        return true;
    });

    stop_timer("parallel for - ");

    ///Test normal for
    start_timer();
    for (auto i = 0; i < values.size(); i++)
        values[i] = calculation(i);

    stop_timer("normal for - ");

    reset();

    ///Test parallel for
    start_timer();

    parallel::_foreach<int>(values.data(), values.size(), [&](int v) {
        values[v] = calculation(v);
        return true;
    });

    stop_timer("parallel foreach - ");

    reset();

    ///Test normal for
    start_timer();

    for (auto v : values)
        values[v] = calculation(v);

    stop_timer("normal foreach - ");
}

void break_test() {
    reset();

    parallel::_foreach<int>(values.data(), values.size(), [&](int v) {
        if (v != 1000)
            values[v] = 0;

        return v != 1000;
    });

    int total_fails = 0;

    for (auto i = 0; i < values.size(); i++)
        if (i != 1000 && values[i] != 0 || i == 1000 && values[i] != 1000)
            total_fails++;

    printf("total fails %i\n", total_fails);
    printf("predicted fails (not exact) %i\n", values.size() - 1000 * parallel::n_threads);
}

void async_test() {
    reset();
    start_timer();
    auto job = parallel::_foreach_async<int>(values.data(), values.size(), [&](int v) {
        if (v == 0)
            std::this_thread::sleep_for(2s);
        calculation(v);
        return true;
    });
    auto stop = stop_timer("async test time -");

    //one of the threads takes 2 seconds making almost impossible for job to finish before main thread reaches this point
    if (stop >= 2000)
        printf("async test failed - %lld", stop);

    parallel::wait_jobs_finish(job);
}

int main() {
    parallel::init(pl::PARALLEL_CORES_ALL);

    values.resize(50000000);

    speed_test();
    break_test();
    async_test();

    parallel::clean();
    values.clear();
    return 0;
}