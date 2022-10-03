#ifndef __PARALLEL__WORKER__
#define __PARALLEL__WORKER__

#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "safe_vector.hpp"
#include <functional>

namespace pl {

    ///type of jobs, if a job has PARALLEL_JOB_TYPE_UNDEFINED type it's because something failed during it's creation or its an invalid job
    enum parallel_job_type {
        PARALLEL_JOB_TYPE_UNDEFINED,
        PARALLEL_JOB_TYPE_FOR,
        PARALLEL_JOB_TYPE_FOREACH
    };

    class parallel_job_base;

    ///class used to allow communication between main thread and workers and between workers
    class parallel_job_parent {
    private:
        std::mutex mtx;
        std::mutex _fq_mtx;
        bool _force_quit = false;
    public:
        //condition variable used to warn main thread that a thread has finished or others threads to quit
        std::condition_variable cv;
        std::vector<parallel_job_base*> jobs;

        //tell threads to force quit current job
        inline void force_quit() {
            if (_force_quit)
                return;

            _fq_mtx.lock();
            _force_quit = true;
            _fq_mtx.unlock();
        }

        //check if it should force quit current job
        inline bool should_force_quit() const { return _force_quit; }

        //check if all jobas have finished
        bool has_finished();
    };

    ///base class for parallel jobs, contains all necessary information and functions
    class parallel_job_base {
    public:
        ///type of job
        parallel_job_type type = PARALLEL_JOB_TYPE_UNDEFINED;

        ///parent to allow communication with main thread and other threads
        parallel_job_parent* parent;

        ///flag to warn the main thread that this thread has finished
        bool finished = false;
        ///start index
        size_t start = -1;
        ///number of items to go through
        size_t size = -1;


        /**
         * parallel job base constructor
         * @param _start start index
         * @param _size number of items to go through
         * @param _parent parent job to communicate with main thread and other threads
         * @param _type type of job
         */
        parallel_job_base(size_t _start, size_t _size, parallel_job_parent* _parent, parallel_job_type _type);

        ///virtual function to start processing
        inline virtual void process() { finished = true; }
    };

    ///main job class where t is the type of object being processed
    template<typename T> class parallel_job : public parallel_job_base {
    public:
        ///pointer to data
        T* data = nullptr;

        ///job callback to process data
        std::function<bool(T)> callback = nullptr;

        /**
         * @param _data pointer to data
         * @param _start start index
         * @param _size number of items to be processed
         * @param _callback callback to process data
         * @param _parent parent job to communicate with other threads and main thread
         **/
        parallel_job(T* _data, size_t _start, size_t _size, std::function<bool(T)>& _callback, parallel_job_parent* _parent = nullptr)
                : parallel_job_base(_start, _size, _parent, _data == nullptr ? PARALLEL_JOB_TYPE_FOR : PARALLEL_JOB_TYPE_FOREACH) {
            data = _data;
            callback = _callback;
        }

        void process() override {
            switch (type) {
                case PARALLEL_JOB_TYPE_UNDEFINED:
                    printf("parallel - error - attempted to execute a undefined job");
                    break;
                case PARALLEL_JOB_TYPE_FOR:
                    for (auto i = 0; i < size; i++) {
                        if (finished || parent->should_force_quit())
                            break;
                        if (!callback(i + start))
                            parent->force_quit();
                    }
                    break;
                case PARALLEL_JOB_TYPE_FOREACH:
                    for (auto i = 0; i < size; i++) {
                        if (finished || parent->should_force_quit())
                            break;
                        if (!callback(data[i + start]))
                            parent->force_quit();
                    }
                    break;
            }
            finished = true;
        }
    };

    ///worker thread
    class parallel_worker {
    private:
        std::thread _thread;
    public:
        ///flag if thread is running or not
        bool running = false;
        ///conditional variable to warn thread that jobs were added or to quit
        std::condition_variable cv;

        ///thread safe vector that holds the jobs
        safe_vector<parallel_job_base*> jobs;

        parallel_worker();
        ~parallel_worker();

        //worker function
        void worker();
    };
}


#endif