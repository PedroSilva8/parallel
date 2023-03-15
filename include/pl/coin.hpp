#ifndef PL_COIN_
#define PL_COIN_

#include <atomic>

namespace pl {
    template<typename T> class coin {
    private:
        coin<T>* m_next = nullptr;
        coin<T>* m_previous = nullptr;
        T* m_value = nullptr;
    public:

        coin() : m_value(nullptr) { };
        explicit coin(T* _value) : m_value(_value) { }
        coin(const coin<T>& _val) : m_value(_val.m_value), m_next(_val.m_next), m_previous(_val.m_previous) { }
        coin(coin<T>&& _val) noexcept : m_value(_val.m_value), m_next(_val.m_next), m_previous(_val.m_previous) { }

        ~coin() {
            if (m_next != nullptr)
                m_next->set_previous(previous());
            if (m_previous != nullptr)
                m_previous->set_next(next());

            delete m_value;
        }

        inline void remove_from_list() {
            if (m_previous != nullptr)
                m_previous->set_next(m_next);
            if (m_next != nullptr)
                m_next->set_previous(m_next);
            m_previous = nullptr;
            m_next = nullptr;
        }

        [[nodiscard]] inline T* value() {
            return m_value;
        }

        inline void set_value(T* _val) {
            m_value = _val;
        }

        [[nodiscard]] inline coin<T>* next() {
            return m_next;
        }

        inline void set_next(coin<T>* _val) {
            if (_val == this)
                return;

            m_next = _val;
            if (_val != nullptr)
                _val->m_previous = this;
        }

        [[nodiscard]] inline coin<T>* previous() {
            return m_previous;
        }

        inline void set_previous(coin<T>* _val) {
            if (_val == this)
                return;

            m_previous = _val;
            if (_val != nullptr)
                _val->m_next = this;
        }
    };
}

#endif