#pragma once

#include "exec/instance/int_alias.hpp"

#include <memory>
#include <vector>

namespace cmsl
{
    namespace exec
    {
        namespace inst
        {
            class instance;

            class list_value
            {
            private:
                using element_t = std::unique_ptr<instance>;
                static constexpr int_t k_special_value{ -1 };

            public:
                list_value(list_value&&) = default;
                list_value& operator=(list_value&&) = default;

                list_value(const list_value& other);
                list_value& operator=(const list_value& other);

                bool operator==(const list_value& other) const;
                bool operator!=(const list_value& other) const;

                void push_back(element_t element);
                void push_back(const list_value& other);
                void push_front(element_t element);
                void push_front(const list_value& other);
                void pop_back();
                void pop_front();

                instance& at(int_t index);
                instance& front();
                instance& back();

                void insert(int_t pos, element_t element);
                void insert(int_t pos, list_value other);

                void erase(int_t pos, int_t count = k_special_value);
                int_t remove(const instance& value, int_t count = k_special_value);
                int_t remove_last(const instance& value, int_t count = k_special_value);
                void clear();

                void resize();

                void sort();
                void reverse();

                int_t min();
                int_t max();

                list_value sublist(int_t pos, int_t count = k_special_value);

                int_t size() const;
                bool empty() const;

                int_t find(const instance& value, int_t pos = k_special_value);
                list_value find_all(const instance& value, int_t pos = k_special_value);

            private:
                std::vector<element_t> m_list;
            };
        }
    }
}
