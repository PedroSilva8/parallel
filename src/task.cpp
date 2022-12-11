#include <pl/task.hpp>
#include <pl/worker.hpp>

using namespace pl;

void pl_job::force_quit() {
    for (auto& task: tasks)
        task->stop();
}