#ifndef __PARALLEL__
#define __PARALLEL__

#include <functional>
#include <thread>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <cmath>

#include "worker.hpp"

namespace pl {

    ///type of parallel n_threads
    enum parallel_cores {
        PARALLEL_CORES_ALL,
        PARALLEL_CORES_ALL_MINUS_ONE,
        PARALLEL_CORES_HALF,
        PARALLEL_CORES_QUARTER,
        PARALLEL_CORES_CUSTOM
    };

    class parallel {
    private:
        /***
         *  @param _start start index
         *  @param _size number of items to process
         *  @param _data pointer to data
         *  @param _callback callback function to process items
         **/
        template<typename T, typename Func> static void create_job(size_t _start, size_t _size, T* _data, Func _callback) {
            if (n_threads == -1) {
                printf("parallel - error - attempted to create a job without calling init");
                return;
            }

            int job_per_core = std::ceil((float)_size / (float)n_threads);

            auto parent = new parallel_job_parent();
            std::vector<parallel_job_base*> jobs;

            int given = 0;

            for (auto i = 0; i < n_threads; i++) {
                auto job = new parallel_job<T>(_data, given + _start, job_per_core + given > _size ? _size - given : job_per_core, _callback, parent);
                given += job_per_core;

                jobs.push_back(job);
                threads[i]->jobs.push_back(job);
                threads[i]->cv.notify_one();
            }

            std::mutex mtx;
            std::unique_lock<std::mutex> l(mtx);
            parent->cv.wait(l, [&]{ return std::all_of(jobs.begin(), jobs.end(), [](parallel_job_base* j) { return j->finished; }); });

            for (auto j : jobs)
                delete j;
            jobs.clear();
        }

    public:
        ///number of threads
        static unsigned int n_threads;

        ///array of pointers to workers
        static parallel_worker** threads;

        /***
         * initialize parallel library
         * @param _cores method to choose number of cores
         * @param _n_cores number of cores to use, ignored if _cores is not custom
         */
        static void init(parallel_cores _cores, unsigned int _n_cores = -1);

        /***
         * threaded for
         * @param _start starting index
         * @param _size number of items to process
         * @param _callback callback to process
         */
        inline static void _for(size_t _start, size_t _size, const std::function<bool(int)>& _callback) {
            create_job<int>(_start, _size, nullptr, _callback);
        }

        /***
         * threaded for (auto value : array)
         * @tparam T type of data being processed
         * @param _data pointer to data
         * @param _size number of items to process
         * @param _callback callback to process
         */
        template<typename T> inline static void _foreach(T* _data, size_t _size, std::function<bool(T)> _callback) {
            create_job(0, _size, _data, _callback);
        }

        ///clean up library
        static void clean();
    };
}


#endif