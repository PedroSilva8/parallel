#include <pl/worker.hpp>
#include <pl/task.hpp>

using namespace pl;

void pl_worker::work() {
    while (m_running) {
        if (m_finished)
            m_finished.wait(true);

        if (!m_running)
            return;

        m_task->process();
        m_finished = true;
        m_finished.notify_all();
    }
}