#include "parallel.hpp"
#include <chrono>
using namespace pl;

unsigned int parallel::n_threads = -1;
parallel_worker** parallel::threads;

void parallel::wait_jobs_finish(parallel_job_parent* _parent, int timeout) {
    if (_parent == nullptr)
        return;

    std::mutex mtx;
    std::unique_lock<std::mutex> l(mtx);
    while (!_parent->has_finished())
        _parent->cv.wait_for(l, std::chrono::milliseconds(timeout) , [&]{ return _parent->has_finished(); } );
}

void parallel::init(parallel_cores _cores, unsigned int _n_cores) {
    n_threads = -1;

    unsigned int physical_cores = std::thread::hardware_concurrency();

    unsigned int final_core = -1;

    switch (_cores) {
        case PARALLEL_CORES_ALL:
            final_core = physical_cores;
            break;
        case PARALLEL_CORES_ALL_MINUS_ONE:
            final_core = physical_cores - 1;
            break;
        case PARALLEL_CORES_HALF:
            final_core = physical_cores / 2;
            if (final_core < 2)
                final_core = 2;
            break;
        case PARALLEL_CORES_QUARTER:
            final_core = physical_cores / 4;
            if (final_core < 2)
                final_core = 2;
            break;
        case PARALLEL_CORES_CUSTOM:
            final_core = _n_cores;
            break;
    }
    n_threads = final_core;

    threads = (parallel_worker**)malloc(n_threads * sizeof(parallel_worker*));

    for (auto i = 0; i < final_core; i++)
        threads[i] = new parallel_worker();
}

void parallel::clean() {
    for (auto i = 0; i < n_threads; i++)
        free(threads[i]);

    threads = nullptr;
}