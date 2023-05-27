#ifndef PL_SAFE_
#define PL_SAFE_

#include <memory>
#include <mutex>

namespace pl {
    template<typename T> class locked_safe {
    private:
        std::shared_ptr<std::mutex> m_mtx;
        std::shared_ptr<T> m_deque;
    public:
        locked_safe(const std::shared_ptr<std::mutex>& _mtx, const std::shared_ptr<T>& _deque) : m_mtx(_mtx), m_deque(_deque) {
            m_mtx->lock();
        }

        locked_safe(locked_safe<T>&& _move) noexcept = default;
        locked_safe& operator=(locked_safe<T>&& _move) noexcept = default;

        locked_safe(const locked_safe<T>& _copy) = delete;
        locked_safe& operator=(const locked_safe<T>& _copy) = delete;

        ~locked_safe() {
            m_mtx->unlock();
        }

        T* get() const {
            return m_deque.get();
        }

        T* operator->() {
            return m_deque.get();
        }
    };

    template<typename T> class safe {
    private:
        std::shared_ptr<std::mutex> m_mtx;
        std::shared_ptr<T> m_deque;
    public:
        safe() : m_mtx(std::make_shared<std::mutex>()), m_deque(std::make_shared<T>()) { }

        [[nodiscard]] inline locked_safe<T> lock() const {
            return locked_safe<T>(m_mtx, m_deque);
        }
    };
}

#endif