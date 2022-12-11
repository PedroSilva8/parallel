#ifndef PL_WORKER_
#define PL_WORKER_

#include <thread>
#include <condition_variable>

namespace pl {
    class pl_task_base;
    class pl_job;

    class pl_worker {
    private:
        std::thread m_thread{};
        pl_task_base* m_task = nullptr;
        std::atomic<bool> m_finished = false;
        pl_job* m_parent = nullptr;
        std::atomic<bool> m_running;
    public:
        std::mutex mtx;
        std::condition_variable cv;

        inline bool finished() { return m_finished; }
        inline void restart() { m_finished = false; }

        explicit pl_worker(pl_job* _parent, pl_task_base* _task) {
            m_task = _task;
            m_finished = false;
            m_running = true;
            m_parent = _parent;
            m_thread = std::thread(&pl_worker::work, this);
        }

        ~pl_worker() {
            std::unique_lock child_lk(mtx);
            m_running = false;
            child_lk.unlock();

            cv.notify_all();
            m_thread.join();
        }

        void work();
    };
}

#endif