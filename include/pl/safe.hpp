#ifndef PL_SAFE_
#define PL_SAFE_

#include <memory>
#include <mutex>

namespace pl {
    template<typename T> class safe {
    private:
        T m_value;
        std::unique_ptr<std::mutex> m_mtx;
    public:
        safe() : m_mtx(std::make_unique<std::mutex>()) { };

        safe(const T& t) : m_mtx(std::make_unique<std::mutex>()) {
            m_value = t;
        }


        safe(const safe<T>& t) : m_mtx(t.m_mtx) {
            m_value = t.m_value;
        }

        T& operator=(const T& t) {
            m_value = t;
            return m_value;
        }

        safe<T>& operator=(const safe<T>& t) {
            m_value = t.m_value;
            m_mtx = t.m_mtx;
            return *this;
        }


        [[nodiscard]] inline T& lock_get() {
            lock();
            return m_value;
        }

        [[nodiscard]] inline T& get() {
            return m_value;
        }

        inline void set(T _value) {
            m_value = _value;
        }

        inline void lock() {
            m_mtx->lock();
        }

        inline void unlock() {
            m_mtx->unlock();
        }

        inline bool is_locked() {
            auto result = m_mtx->try_lock();
            if (result)
                m_mtx->unlock();
            return result;
        }
    };
}

#endif