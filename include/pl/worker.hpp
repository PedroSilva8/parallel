#ifndef PL_WORKER_
#define PL_WORKER_

#include <thread>
#include "task.hpp"

namespace pl {
    class pl_worker {
    private:
        std::thread m_thread{};
        pl_task_base* m_task = nullptr;
        std::atomic<bool> m_finished = false;
        pl_job* m_parent = nullptr;
    public:
        inline bool finished() { return m_finished; }

        explicit pl_worker(pl_job* _parent, pl_task_base* _task) {
            m_task = _task;
            m_finished = false;
            m_parent = _parent;
            m_thread = std::thread(&pl_worker::work, this);
        }

        ~pl_worker() {
            m_thread.join();
        }

        inline void work() {
            m_task->process();
            m_finished = true;
            std::unique_lock lk(m_parent->mtx);
            std::notify_all_at_thread_exit(m_parent->cv, std::move(lk));
        }
    };
}

#endif