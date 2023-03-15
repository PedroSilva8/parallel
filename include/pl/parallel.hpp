#ifndef PL_PARALLEL_
#define PL_PARALLEL_

#include <memory>
#include <thread>
#include <deque>
#include <functional>
#include <condition_variable>
#include <latch>

namespace pl {

    enum PL_TASK_WORK_TYPE {
        PL_TASK_WORK_TYPE_FOR,
        PL_TASK_WORK_TYPE_FOREACH
    };

    struct pl_task_data {
        std::condition_variable m_cv;
        std::atomic<bool> running = true;
        std::atomic<bool> complete = true;
        std::mutex mtx;

        std::atomic<size_t> index = 0;
        std::atomic<size_t> size = 0;
        std::deque<std::jthread> workers;
        std::unique_ptr<std::latch> latch;

        ~pl_task_data() {
            running = false;
            m_cv.notify_all();
            for (size_t i = 0; i < workers.size(); i++) {
                workers.front().join();
                workers.pop_front();
            }
        }
    };

    template<typename T> class pl_task {
    private:
        std::shared_ptr<pl_task_data> m_task_data;
        std::atomic<T*> m_data;
        size_t m_start;
        PL_TASK_WORK_TYPE m_work_type;

        std::function<bool(T&)> m_callback;

        template<typename T2> void process() {
            size_t i = m_task_data->index++;

            while (i < m_task_data->size) {
                if (!m_callback(m_data[i]))
                    m_task_data->index.exchange(m_task_data->size);
                i = m_task_data->index++;
            }
        }

        void work() {
            while (m_task_data->running) {
                while (m_task_data->complete && m_task_data->running) {
                    std::unique_lock<std::mutex> lock(m_task_data->mtx);
                    m_task_data->m_cv.wait(lock, [&] { return !m_task_data->complete || !m_task_data->running.load(); });
                }

                if (!m_task_data->running)
                    return;

                process<T>();

                m_task_data->latch->arrive_and_wait();
                m_task_data->complete.exchange(true);
                m_task_data->m_cv.notify_all();
            }
        }

    public:
        pl_task(size_t _cores, const std::function<bool(T&)>& _callback, T* _data, size_t _size) : m_task_data(new pl_task_data()), m_work_type(PL_TASK_WORK_TYPE_FOREACH) {
            m_data = _data;
            m_callback = _callback;
            m_task_data->size.exchange(_size);
            for (size_t i = 0; i < _cores; i++)
                m_task_data->workers.push_back(std::jthread(&pl_task::work, this));
            m_task_data->latch = std::make_unique<std::latch>(_cores);
        }

        pl_task(size_t _cores, const std::function<bool(T&)>& _callback, size_t _start, size_t _size) : m_task_data(new pl_task_data()), m_work_type(PL_TASK_WORK_TYPE_FOR) {
            m_start = _start;
            m_callback = _callback;
            m_task_data->size.exchange(_size);
            for (size_t i = 0; i < _cores; i++)
                m_task_data->workers.push_back(std::jthread(&pl_task::work, this));
            m_task_data->latch = std::make_unique<std::latch>(_cores);
        }

        pl_task(const pl_task& _copy) {
            m_data = _copy.m_data.load();
            m_callback = _copy.m_callback;
            m_task_data = _copy.m_task_data;
            m_work_type = _copy.m_work_type;
            m_start = _copy.m_start;
        }

        pl_task& operator=(const pl_task& _copy) {
            if (this == &_copy)
                return *this;

            m_data = _copy.m_data.load();
            m_callback = _copy.m_callback;
            m_task_data = _copy.m_task_data;
            m_work_type = _copy.m_work_type;
            m_start = _copy.m_start;
            return *this;
        }

        inline pl_task& start() {
            switch (m_work_type) {
                case PL_TASK_WORK_TYPE_FOR:
                    m_task_data->index.exchange(m_start);
                    break;
                case PL_TASK_WORK_TYPE_FOREACH:
                    m_task_data->index.exchange(0);
                    break;
            }

            m_task_data->latch = std::make_unique<std::latch>(m_task_data->workers.size());
            m_task_data->complete.exchange(false);
            m_task_data->m_cv.notify_all();
            return *this;
        }

        inline pl_task& wait() {
            while (!m_task_data->complete.load()) {
                std::unique_lock<std::mutex> lock(m_task_data->mtx);
                m_task_data->m_cv.wait(lock, [&] { return m_task_data->complete.load(); });
            }
            return *this;
        }
    };

    template<> template<> void pl_task<size_t>::process<size_t>() {
        size_t i;
        switch (m_work_type) {
            case PL_TASK_WORK_TYPE_FOR:
                i = m_task_data->index++;
                while (i < m_task_data->size) {
                    if (!m_callback(i))
                        m_task_data->index.exchange(m_task_data->size);
                    i = m_task_data->index++;
                }
                return;
            case PL_TASK_WORK_TYPE_FOREACH:
                i = m_task_data->index++;

                while (i < m_task_data->size) {
                    if (!m_callback(m_data[i]))
                        m_task_data->index.exchange(m_task_data->size);
                    i = m_task_data->index++;
                }
                return;
        }
    }

    template<typename T> pl_task<T> async_foreach(T* _data, size_t _size, const std::function<bool(T&)>& _callback, size_t _cores = std::thread::hardware_concurrency()) {
        pl_task<T> task(_cores, _callback, _data, _size);
        return task;
    }

    template<typename T> void foreach(T* _data, size_t _size, const std::function<bool(T&)>& _callback, size_t _cores = std::thread::hardware_concurrency()) {
        pl_task<T> task(_cores, _callback, _data, _size);
        task.start().wait();
    }

    pl_task<size_t> async_for(size_t _start, size_t _size, const std::function<bool(size_t&)>& _callback, size_t _cores = std::thread::hardware_concurrency()) {
        pl_task<size_t> task(_cores, _callback, _start, _size);
        return task;
    }

    void _for(size_t _start, size_t _size, const std::function<bool(size_t&)>& _callback, size_t _cores = std::thread::hardware_concurrency()) {
        pl_task<size_t> task(_cores, _callback, _start, _size);
        task.start().wait();
    }
}

#endif