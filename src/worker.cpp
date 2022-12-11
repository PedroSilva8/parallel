#include <pl/worker.hpp>
#include <pl/task.hpp>

using namespace pl;

void pl_worker::work() {
    while (m_running) {
        {
            std::unique_lock child_lk(mtx);
            cv.wait(child_lk, [&] { return !m_finished || !m_running; });
        }

        if (!m_running)
            return;

        m_finished = false;
        m_task->process();

        {
            std::unique_lock child_lk(mtx);
            m_finished = true;
        }

        cv.notify_all();
    }
}