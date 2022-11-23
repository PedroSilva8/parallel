#ifndef PL_PARALLEL_
#define PL_PARALLEL_

#include <functional>
#include <cmath>
#include "worker.hpp"

namespace pl {
    enum pl_cores {
        PL_CORES_ALL,
        PL_CORES_ALL_MINUS_ONE,
        PL_CORES_HALF,
        PL_CORES_QUARTER,
        PL_CORES_SINGLE,
    };

    template<typename T> pl_job* create_job(pl_job_task _task_type, size_t _start, size_t _size, T* _data, const std::function<bool(T&)>& _callback, pl_cores _cores) {
        auto new_job = new pl_job();
        size_t physical_cores = std::thread::hardware_concurrency();

        size_t actual_cores;

        switch (_cores) {
            case PL_CORES_ALL_MINUS_ONE:
                actual_cores -= 1;
                break;
            case PL_CORES_HALF:
                actual_cores = std::min(physical_cores / 2, (size_t)1);
                break;
            case PL_CORES_QUARTER:
                actual_cores = std::min(physical_cores / 4, (size_t)1);
                break;
            case PL_CORES_SINGLE:
                actual_cores = 1;
                break;
            case PL_CORES_ALL:
                actual_cores = physical_cores;
                break;
        }

        size_t tasks_per_core = std::ceil((long double)_size / (long double)actual_cores);

        size_t tasks_given = 0;

        for (auto i = 0; i < actual_cores; i++) {
            auto task = new pl_task<T>(new_job, _task_type, tasks_given + _start, tasks_per_core + tasks_given > _size ? _size - tasks_given : tasks_per_core, _data, _callback);
            tasks_given += tasks_per_core;

            new_job->tasks.push_back(task);

            if (tasks_given >= _size)
                break;
        }

        return  new_job;
    }

    /**
    * Create a for job, blocks executing thread while waiting
    * @param _start where to start
    * @param _length length of array
    * @param _callback callback to execute each instance
    * @param _cores core type
    */
    void _for(size_t _start, size_t _length, const std::function<bool(size_t&)>& _callback, pl_cores _cores = PL_CORES_ALL) {
        size_t i = 0;
        auto new_job = create_job<size_t>(PL_TASK_TYPE_FOR, _start, _length, &i, _callback, _cores);
        new_job->start();
        new_job->wait();
        new_job->clean();
        delete new_job;
    }

    /**
    * Create a foreach job, blocks executing thread while waiting
    * @tparam T Data type
    * @param _data pointer to data
    * @param _length length of array
    * @param _callback callback to execute each instance
    * @param _cores core type
    */
    template<typename T> void _foreach(T* _data, size_t _length, const std::function<bool(T&)>& _callback, pl_cores _cores = PL_CORES_ALL) {
        auto new_job = create_job<T>(PL_TASK_TYPE_FOREACH, 0, _length, _data, _callback, _cores);
        new_job->start();
        new_job->wait();
        new_job->clean();
    }

    /**
     * Create a async for job, useful when repeating tasks
     * @param _start where to start
     * @param _length length of array
     * @param _callback callback to execute each instance
     * @param _cores core type
     * @return returns pointer to job don't forget to delete it
     */
    pl_job* async_for(size_t _start, size_t _length, const std::function<bool(size_t&)>& _callback, pl_cores _cores = PL_CORES_ALL) {
        size_t i = 0;
        auto new_job = create_job<size_t>(PL_TASK_TYPE_FOR, _start, _length, &i, _callback, _cores);
        new_job->start();
        return new_job;
    }

    /**
     * Create a async foreach job, useful when repeating tasks
     * @tparam T Data type
     * @param _data pointer to data
     * @param _length length of array
     * @param _callback callback to execute each instance
     * @param _cores core type
     * @return returns pointer to job don't forget to delete it
     */
    template<typename T> pl_job* async_foreach(T* _data, size_t _length, const std::function<bool(T&)>& _callback, pl_cores _cores = PL_CORES_ALL) {
        auto new_job = create_job<T>(PL_TASK_TYPE_FOREACH, 0, _length, _data, _callback, _cores);
        new_job->start();
        return new_job;
    }
}

#endif