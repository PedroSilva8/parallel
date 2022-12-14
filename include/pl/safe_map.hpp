#ifndef PL_SAFE_MAP_
#define PL_SAFE_MAP_

#include <map>
#include <mutex>
#include <algorithm>

namespace pl {

    /***
     * map wrapper to guarantee thread safety
     * @tparam T key type
     * @tparam C value type
     */
    template<typename T, typename C> struct safe_map {
    private:
        mutable std::mutex m_mtx;
        std::map<T, C> m_data;
    public:


        void insert(T _key, C _value) {
            std::lock_guard<std::mutex> l(m_mtx);
            m_data.insert(std::pair<T, C>(_key, _value));
        }

        bool has(T _key) {
            std::lock_guard<std::mutex> l(m_mtx);
            return m_data.find(_key) != m_data.end();
        }

        typename std::map<T, C>::const_iterator begin() const {
            std::lock_guard<std::mutex> l(m_mtx);
            return m_data.begin();
        }

        typename std::map<T, C>::const_iterator end() const {
            std::lock_guard<std::mutex> l(m_mtx);
            return m_data.end();
        }

        C &at(T _key) const {
            std::lock_guard<std::mutex> l(m_mtx);
            return m_data[_key];
        }

        C &operator[](T _key) {
            std::lock_guard<std::mutex> l(m_mtx);
            return m_data[_key];
        }

        bool empty() {
            std::lock_guard<std::mutex> l(m_mtx);
            return m_data.empty();
        }

        void erase(T _value) {
            std::lock_guard<std::mutex> l(m_mtx);
            m_data.erase(_value);
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

        safe_map& operator=(const safe_map& other) {
            m_data = other.m_data;
            return *this;
        }
    };
}
#endif