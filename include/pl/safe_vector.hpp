#ifndef PL_SAFE_VECTOR_
#define PL_SAFE_VECTOR_

#include <mutex>
#include <vector>
#include <memory>

namespace pl {

    ///wrapper vector to guarantee thread safety
    template<typename T> struct safe_vector : std::vector<T> {
    private:
        std::shared_ptr<std::mutex> m_mtx;
    public:
        safe_vector() : m_mtx(std::make_shared<std::mutex>()) { };
        safe_vector(const safe_vector& _other) { std::copy(this->begin(), this->end(), _other.begin()); };


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

        void erase_at(size_t index) {
            std::vector<T>::erase(this->begin() + index);
        }
    };
}

#endif