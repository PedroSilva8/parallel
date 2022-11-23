#ifndef PL_TASK_
#define PL_TASK_

#include "safe_vector.hpp"
#include <atomic>
#include <functional>
#include <condition_variable>

namespace pl {

    enum pl_job_task {
        PL_TASK_TYPE_UNDEFIEND,
        PL_TASK_TYPE_FOR,
        PL_TASK_TYPE_FOREACH
    };

    struct pl_task_base;
    struct pl_worker;

    class pl_job {
    private:
        safe_vector<pl_worker*> m_workers;
    public:
        std::mutex mtx;
        safe_vector<pl_task_base*> tasks;
        std::condition_variable cv;

        ~pl_job();

        void force_quit();

        void start();
        void wait();
        void clean();
    };

    class pl_task_base {
    protected:
        pl_job_task m_task_type = PL_TASK_TYPE_UNDEFIEND;
        std::atomic<bool> m_stop = false;

        size_t m_start = 0;
        size_t m_length = 0;
    public:
        inline void stop() { m_stop = true; }
        virtual void process() { }
    };

    template<typename T> class pl_task : public pl_task_base {
    private:
        T* m_data;
        pl_job* m_job;

        std::function<bool(T&)> m_callback;
    public:
        pl_task(pl_job* _job, pl_job_task _task, size_t _start, size_t _length, T* _data, const std::function<bool(T&)>& _callback) {
            m_task_type = _task;
            m_start = _start;
            m_length = _length;
            m_callback = _callback;
            m_data = _data;
            m_job = _job;
        }

        void process() override {
            switch (m_task_type) {
                case PL_TASK_TYPE_FOREACH:
                    for (size_t i = m_start; i < m_start + m_length; i++)
                        if (m_stop || !m_callback(m_data[i])) {
                            m_job->force_quit();
                            break;
                        }
                    break;
                default:
                    printf("Parallel - Error - Attempted to process a invalid task");
                    break;
            }
        }
    };

    template<> class pl_task<size_t> : public pl_task_base {
    private:
        size_t* m_data;
        pl_job* m_job;

        std::function<bool(size_t&)> m_callback;
    public:
        pl_task(pl_job* _job, pl_job_task _task, size_t _start, size_t _length, size_t* _data, const std::function<bool(size_t&)>& _callback) {
            m_task_type = _task;
            m_start = _start;
            m_length = _length;
            m_callback = _callback;
            m_data = _data;
            m_job = _job;
        }

        void process() override {
            switch (m_task_type) {
                case PL_TASK_TYPE_FOR:
                    for (size_t i = m_start; i < m_start + m_length; i++)
                        if (m_stop || !m_callback(i)) {
                            m_job->force_quit();
                            break;
                        }
                    break;
                case PL_TASK_TYPE_FOREACH:
                    for (size_t i = m_start; i < m_start + m_length; i++)
                        if (m_stop || !m_callback(m_data[i])) {
                            m_job->force_quit();
                            break;
                        }
                    break;
                default:
                    printf("Parallel - Error - Attempted to process a invalid task");
                    break;
            }
        }
    };
}

#endif