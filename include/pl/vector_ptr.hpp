#ifndef MARS_VECTOR_PTR_
#define MARS_VECTOR_PTR_

#include <memory>
#include <functional>

namespace pl {

    template<typename T> struct vector_ptr_item : public std::enable_shared_from_this<vector_ptr_item<T>> {
    private:
        std::shared_ptr<vector_ptr_item<T>> m_previous = nullptr;
        std::shared_ptr<vector_ptr_item<T>> m_next = nullptr;
        std::shared_ptr<T> m_value = nullptr;
    public:

        explicit vector_ptr_item(const std::shared_ptr<T>& _val) {
            m_value = _val;
        }

        T* value() const {
            return m_value.get();
        }

        std::shared_ptr<vector_ptr_item> next() const {
            return m_next;
        }

        void set_next(const std::shared_ptr<vector_ptr_item<T>>& _next) {
            m_next = _next;
            _next->m_previous = this->shared_from_this();
        }

        std::shared_ptr<vector_ptr_item> previous() const {
            return m_previous;
        }

        void set_previous(const std::shared_ptr<vector_ptr_item<T>>& _previous) {
            m_previous = _previous;
            _previous->m_next = this->shared_from_this();
        }
    };

    template<typename T> class vector_ptr {
    private:
        std::shared_ptr<vector_ptr_item<T>> m_head;
        std::shared_ptr<vector_ptr_item<T>> m_tail;
        size_t m_size;
    public:
        void push_back(const std::shared_ptr<T>& _value) {
            m_size++;

            if (m_tail != nullptr) {
                auto new_value = std::make_shared<vector_ptr_item<T>>(_value);
                new_value->set_previous(m_tail);
                m_tail = new_value;
            }
            else
                m_head = m_tail = std::make_shared<vector_ptr_item<T>>(_value);
        }

        void push_back(T& _value) {
            push_back(std::make_shared<T>(_value));
        }

        void push_front(T* _value) {
            if (m_tail != nullptr) {
                m_size++;
                auto new_value = std::make_shared<vector_ptr_item<T>>(_value);
                new_value->set_previous(m_tail);
                m_tail = new_value;
            }

            m_head = m_tail = std::make_shared<vector_ptr_item<T>>(_value);
        }

        void join_back(const vector_ptr<T>& _right) {
            _right.m_head->set_previous(m_tail);
        }

        void clear() {
            m_head = m_tail = nullptr;
            m_size = 0;
        }

        void _for(const std::function<void(T*)>& _func) const {
            auto head = m_head;
            while (head != nullptr) {
                _func(head->value());
                head = head->next();
            }
        }
    };
}

#endif