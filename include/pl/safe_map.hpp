#ifndef PL_SAFE_MAP_
#define PL_SAFE_MAP_

#include <map>
#include <mutex>
#include <algorithm>
#include <memory>

namespace pl {

    template<typename T, typename C> class locked_map {
    private:
        std::shared_ptr<std::mutex> m_mtx;
        std::shared_ptr<std::map<T, C>> m_map;
    public:
        locked_map(const std::shared_ptr<std::mutex>& _mtx, const std::shared_ptr<std::map<T, C>>& _map) : m_mtx(_mtx), m_map(_map) {
            m_mtx->lock();
        }

        ~locked_map() {
            m_mtx->unlock();
        }

        std::map<T, C>* get() const {
            return m_map.get();
        }

        std::map<T, C>* operator->() {
            return m_map.get();
        }
    };

    /***
     * map wrapper to guarantee thread safety
     * @tparam T key type
     * @tparam C get type
     */
    template<typename T, typename C> class safe_map {
    private:
        std::shared_ptr<std::mutex> m_mtx;
        std::shared_ptr<std::map<T, C>> m_map;
    public:
        safe_map() : m_mtx(std::make_shared<std::mutex>()), m_map(std::make_shared<std::map<T, C>>()) { };

        [[nodiscard]] inline locked_map<T, C> lock() const {
            return locked_map<T, C>(m_mtx, m_map);
        }
    };
}
#endif