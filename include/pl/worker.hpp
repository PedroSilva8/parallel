#ifndef PL_WORKER_
#define PL_WORKER_

#include <thread>
#include <condition_variable>

namespace pl {
    class pl_task_base;
    class pl_job;

    class pl_worker {
    private:
        std::thread m_thread;
        pl_task_base* m_task = nullptr;
        std::atomic<bool> m_finished = true;
        pl_job* m_parent = nullptr;
        std::atomic<bool> m_running = true;
    public:

        inline void wait(bool _old) { m_finished.wait(_old); }

        inline bool finished() { return m_finished; }
        inline void restart() { m_finished.exchange(false); m_finished.notify_all(); }

        explicit pl_worker(pl_job* _parent, pl_task_base* _task) : m_task(_task), m_parent(_parent), m_thread{} {
            m_thread = std::thread{&pl_worker::work, this};
        }

        ~pl_worker() {
            m_running = false;
            m_finished = true;
            m_finished.notify_all();
            m_thread.join();
        }

        void work();
    };
}

#endif