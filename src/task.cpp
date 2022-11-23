#include <pl/task.hpp>
#include <pl/worker.hpp>

using namespace pl;

pl_job::~pl_job() {
    for (auto& task : tasks)
        delete task;
}

void pl_job::force_quit() {
    for (auto& task: tasks)
        task->stop();
}

void pl_job::load() {
    for (auto& task : tasks)
        m_workers.push_back(new pl_worker(this, task));
}

void pl_job::start() {
    for (auto& worker : m_workers) {
        {
            std::unique_lock lk(worker->mtx);
            worker->restart();
        }
        worker->cv.notify_all();
    }
}

void pl_job::wait() {
    for (auto& worker : m_workers) {
        std::unique_lock lk(worker->mtx);
        worker->cv.wait(lk, [&]{ return worker->finished(); });
    }
}

void pl_job::clean() {
    for (auto& worker : m_workers)
        delete worker;
}