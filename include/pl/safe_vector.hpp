#ifndef PL_SAFE_VECTOR_
#define PL_SAFE_VECTOR_

#include <mutex>
#include <vector>
#include <memory>

namespace pl {

    template<typename T> class locked_vector {
    private:
        std::shared_ptr<std::mutex> m_mtx;
        std::shared_ptr<std::vector<T>> m_vec;
    public:
        locked_vector(const std::shared_ptr<std::mutex>& _mtx, const std::shared_ptr<std::vector<T>>& _vec) : m_mtx(_mtx), m_vec(_vec) {
            m_mtx->lock();
        }

        ~locked_vector() {
            m_mtx->unlock();
        }

        std::vector<T>* get() const {
            return m_vec.get();
        }

        std::vector<T>* operator->() {
            return m_vec.get();
        }
    };

    ///wrapper vector to guarantee thread safety
    template<typename T> class safe_vector {
    private:
        std::shared_ptr<std::mutex> m_mtx;
        std::shared_ptr<std::vector<T>> m_vec;
    public:
        safe_vector() : m_mtx(std::make_shared<std::mutex>()), m_vec(std::make_shared<std::vector<T>>()) { }

        [[nodiscard]] inline locked_vector<T> lock() const {
            return locked_vector<T>(m_mtx, m_vec);
        }
    };
}

#endif