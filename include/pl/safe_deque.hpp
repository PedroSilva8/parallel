#ifndef MARS_SAFE_DEQUE_
#define MARS_SAFE_DEQUE_

#include <deque>
#include <mutex>

namespace pl {
    template<typename T> class locked_deque {
    private:
        std::shared_ptr<std::mutex> m_mtx;
        std::shared_ptr<std::deque<T>> m_deque;
    public:
        locked_deque(const std::shared_ptr<std::mutex>& _mtx, const std::shared_ptr<std::deque<T>>& _deque) : m_mtx(_mtx), m_deque(_deque) {
            m_mtx->lock();
        }

        ~locked_deque() {
            m_mtx->unlock();
        }

        std::deque<T>* get() const {
            return m_deque.get();
        }

        std::deque<T>* operator->() {
            return m_deque.get();
        }
    };

    template<typename T> class safe_deque {
    private:
        std::shared_ptr<std::mutex> m_mtx;
        std::shared_ptr<std::deque<T>> m_deque;
    public:
        safe_deque() : m_mtx(std::make_shared<std::mutex>()), m_deque(std::make_shared<std::deque<T>>()) { }

        [[nodiscard]] inline locked_deque<T> lock() const {
            return locked_deque<T>(m_mtx, m_deque);
        }
    };
}

#endif