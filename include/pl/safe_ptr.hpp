#ifndef PL_SAFE_PTR_
#define PL_SAFE_PTR_

#include <memory>

namespace pl {
    template<typename T> class safe_ptr {
    private:
        std::shared_ptr<T> m_value;
        std::shared_ptr<std::mutex> m_mtx;
    public:
        safe_ptr() : m_mtx(std::make_shared<std::mutex>()) { };

        explicit safe_ptr(const T& t) : m_mtx(std::make_shared<std::mutex>()) {
            m_value = std::make_shared<T>(t);
        }

        safe_ptr(const safe_ptr<T>& t) : m_mtx(t.m_mtx) {
            m_value = t.m_value;
        }

        safe_ptr<T>& operator=(const T& t) {
            m_value = t;
            return *this;
        }

        safe_ptr<T>& operator=(const safe_ptr<T>& t) {
            if (this == &t)
                return *this;

            m_value = t.m_value;
            m_mtx = t.m_mtx;
            return *this;
        }


        [[nodiscard]] inline std::shared_ptr<T>& lock_get() {
            lock();
            return m_value;
        }

        [[nodiscard]] inline std::shared_ptr<T>& get() {
            return m_value;
        }

        inline void set(T _value) {
            m_value = std::make_shared<T>(_value);
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