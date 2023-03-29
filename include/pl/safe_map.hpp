#ifndef PL_SAFE_MAP_
#define PL_SAFE_MAP_

#include <map>
#include <mutex>
#include <algorithm>
#include <memory>

namespace pl {

    /***
     * map wrapper to guarantee thread safety
     * @tparam T key type
     * @tparam C get type
     */
    template<typename T, typename C> struct safe_map : std::map<T, C> {
    private:
        std::shared_ptr<std::mutex> m_mtx;
    public:
        safe_map() : m_mtx(std::make_shared<std::mutex>()) { };

        inline bool safe_contains(const T& key) {
            lock();
            auto result = this->contains(key);
            unlock();
            return result;
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