#include "worker.hpp"
#include "parallel.hpp"

using namespace pl;

bool parallel_job_parent::has_finished() {
    return std::all_of(jobs.begin(), jobs.end(), [](parallel_job_base* j) { return j->finished; });
}

parallel_job_base::parallel_job_base(size_t _start, size_t _size, parallel_job_parent* _parent, parallel_job_type _type) {
    start = _start;
    size = _size;
    type = _type;
    parent = _parent;
}

parallel_worker::parallel_worker() {
    running = true;
    _thread = std::thread(&parallel_worker::worker, this);
}

parallel_worker::~parallel_worker() {
    running = false;
    cv.notify_all();
    _thread.join();

    if (!jobs.empty())
        for (auto j : jobs)
            delete j;
    jobs.clear();
}

void parallel_worker::worker() {
    std::mutex mtx;
    std::unique_lock<std::mutex> l(mtx);
    while (running) {
        cv.wait(l, [&]{ return !jobs.empty() || !running; });

        while (!jobs.empty()) {
            auto j = jobs.at(0);
            j->process();
            if (j->parent != nullptr)
                j->parent->cv.notify_all();
            jobs.erase(j);
        }
    }
    l.unlock();
}