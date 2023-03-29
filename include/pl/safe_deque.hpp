#ifndef MARS_SAFE_DEQUE_
#define MARS_SAFE_DEQUE_

#include <deque>
#include <mutex>

namespace pl {
    template<typename T> class safe_deque : public std::deque<T> {
    private:
        std::mutex m_mtx;
    public:
        void lock() {
            m_mtx.lock();
        }

        void unlock() {
            m_mtx.unlock();
        }
    };
}

#endif