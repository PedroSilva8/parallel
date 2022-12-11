#ifndef PL_SAFE_VECTOR_
#define PL_SAFE_VECTOR_

#include <mutex>
#include <vector>

namespace pl {

    ///wrapper vector to guarantee thread safety
    template<typename T> struct safe_vector {
    private:
        mutable std::mutex m_mtx;
        std::vector<T> m_data;
    public:

        safe_vector() = default;

        safe_vector(const safe_vector& _vec) {
            m_data = _vec.m_data;
        }

        ~safe_vector() {
            m_data.clear();
            m_data.shrink_to_fit();
        }

        void push_back(T _value) {
            std::lock_guard<std::mutex> l(m_mtx);
            m_data.push_back(_value);
        }

        typename std::vector<T>::const_iterator begin() const {
            std::lock_guard<std::mutex> l(m_mtx);
            return m_data.begin();
        }

        typename std::vector<T>::const_iterator end() const {
            std::lock_guard<std::mutex> l(m_mtx);
            return m_data.end();
        }

        T at(size_t _index) const {
            std::lock_guard<std::mutex> l(m_mtx);
            return m_data[_index];
        }

        T operator[](size_t _index) const {
            std::lock_guard<std::mutex> l(m_mtx);
            return m_data[_index];
        }

        bool empty() {
            std::lock_guard<std::mutex> l(m_mtx);
            return m_data.empty();
        }

        void erase(T _value) {
            std::lock_guard<std::mutex> l(m_mtx);
            m_data.erase(find(m_data.begin(), m_data.end(), _value));
        }

        void resize(size_t _new_size) {
            std::lock_guard<std::mutex> l(m_mtx);
            m_data.resize(_new_size);
        }

        size_t size() const {
            return m_data.size();
        }

        void erase_at(size_t index) {
            std::lock_guard<std::mutex> l(m_mtx);
            m_data.erase(m_data.begin() + index);
        }

        void clear() {
            std::lock_guard<std::mutex> l(m_mtx);
            m_data.clear();
        }

        safe_vector& operator=(const safe_vector& other) {
            m_data = other.m_data;
            return *this;
        }
    };
}

#endif