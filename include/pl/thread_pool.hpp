#ifndef PL_THREAD_POOL_
#define PL_THREAD_POOL_

#include <queue>
#include <thread>
#include <functional>
#include <atomic>
#include <condition_variable>

namespace pl {

    struct thread_pool_task {
    private:
        std::shared_ptr<std::mutex> m_mtx;
        std::shared_ptr<std::condition_variable> m_cv;
        std::shared_ptr<std::atomic<bool>> m_done;
        std::function<void()> m_work;
    public:

        explicit thread_pool_task() = default;

        explicit thread_pool_task(const std::function<void()>& _work) : m_done(new std::atomic<bool>(false)), m_work(_work), m_mtx(new std::mutex()), m_cv(new std::condition_variable()) { }

        thread_pool_task(const thread_pool_task& _move) = default;
        thread_pool_task(thread_pool_task&& _move) noexcept : m_work(std::move(_move.m_work)), m_cv(std::move(_move.m_cv)), m_mtx(std::move(_move.m_mtx)), m_done(std::move(_move.m_done)) { }


        inline void work() {
            m_work();
            m_done->exchange(true);
        }

        inline void wait() const {
            while (!m_done) {
                std::unique_lock<std::mutex> lock(*m_mtx);
                m_cv->wait(lock, [&] { return m_done->load(); });
            }
        }

        inline void notify() {
            m_cv->notify_all();
        }

        thread_pool_task& operator=(thread_pool_task&& _move)  noexcept {
            if (this == &_move)
                return *this;

            m_work = (std::move(_move.m_work));
            m_cv = (std::move(_move.m_cv));
            m_mtx = (std::move(_move.m_mtx));
            m_done = (std::move(_move.m_done));
            return *this;
        }

        thread_pool_task& operator=(const thread_pool_task& _move) = default;
    };

    class thread_pool {
    protected:
        std::atomic<bool> m_running = true;
        mutable std::mutex m_mtx_list;

        std::condition_variable m_cv;

        std::deque<std::jthread> m_workers;
        std::deque<thread_pool_task> m_tasks;

        void worker() {
            while (m_running.load()) {
                while (m_tasks.empty() && m_running) {
                    std::unique_lock<std::mutex> lock(m_mtx_list);
                    m_cv.wait(lock, [&] { return !m_tasks.empty() || !m_running.load(); });
                }

                if (!m_running)
                    break;

                while (!m_tasks.empty()) {
                    thread_pool_task current;
                    {
                        std::unique_lock<std::mutex> l(m_mtx_list);

                        if (m_tasks.empty())
                            continue;

                        current = m_tasks.front();
                        m_tasks.pop_front();
                    }

                    current.work();
                    current.notify();
                }
            }
        }
    public:
        explicit thread_pool(size_t _cores = std::thread::hardware_concurrency()) {
            for (size_t i = 0; i < _cores; i++)
                m_workers.emplace_back(&thread_pool::worker, this);
        }

        ~thread_pool() {
            m_running = false;
            m_cv.notify_all();
            for (size_t i = 0; i < m_workers.size(); i++) {
                m_workers.front().join();
                m_workers.pop_front();
            }
        }

        inline bool empty() const {
            return m_tasks.empty();
        }

        template<typename F, typename ...A> thread_pool_task add_work(F&& _task, A&&... args) {
            auto task = thread_pool_task(std::bind(std::forward<F>(_task), std::forward<A>(args)...));
            m_mtx_list.lock();
            auto result = m_tasks.emplace_back(task);
            m_mtx_list.unlock();


            m_cv.notify_all();
            return result;
        }
    };
}

#endif