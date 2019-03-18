#include "exec/instance/list_value.hpp"

#include "exec/instance/instance.hpp"
#include "list_value.hpp"


#include <algorithm>

namespace cmsl
{
    namespace exec
    {
        namespace inst
        {
            list_value &list_value::operator=(list_value &&) = default;
            list_value::list_value(list_value &&) = default;

            list_value::~list_value()
            {}

            list_value::list_value(container_t values)
                : m_list{ std::move(values) }
            {}

            list_value::list_value(const list_value &other)
            {
                append_with_copy(other);
            }

            list_value &list_value::operator=(const list_value &other)
            {
                if(this == &other)
                {
                    return *this;
                }

                m_list.clear();
                append_with_copy(other);

                return *this;
            }

            bool list_value::operator==(const list_value &other) const
            {
                const auto pred = [](const auto& lhs_instance,
                                     const auto& rhs_instance)
                {
                    return lhs_instance->get_value_cref() == rhs_instance->get_value_cref();
                };

                return std::equal(std::cbegin(m_list), std::cend(m_list),
                                  std::cbegin(other.m_list), std::cend(other.m_list),
                                  pred);
            }

            bool list_value::operator!=(const list_value &other) const
            {
                return !(*this == other);
            }

            bool list_value::operator<(const list_value &other) const
            {
                const auto pred = [](const auto& lhs_instance,
                                     const auto& rhs_instance)
                {
                    return lhs_instance->get_value_cref() < rhs_instance->get_value_cref();
                };

                return std::lexicographical_compare(cbegin(), cend(),
                                                    other.cbegin(), other.cend(),
                                                    pred);
            }

            bool list_value::operator<=(const list_value &other) const
            {
                return *this == other || *this < other;
            }

            bool list_value::operator>(const list_value &other) const
            {
                return !(*this <= other);
            }

            bool list_value::operator>=(const list_value &other) const
            {
                return *this == other || *this > other;
            }

            void list_value::append_with_copy(const list_value& other)
            {
                append_with_copy_to(other, m_list);
            }

            void list_value::append_with_copy_to(const list_value &other, container_t& container) const
            {
                std::transform(std::cbegin(other.m_list), std::cend(other.m_list),
                               std::back_inserter(container),
                               [](const auto& element)
                               {
                                   return element->copy();
                               });
            }

            void list_value::push_back(list_value::element_t element)
            {
                m_list.emplace_back(std::move(element));
            }

            void list_value::push_back(const list_value &other)
            {
                append_with_copy(other);
            }

            void list_value::push_front(element_t element)
            {
                m_list.emplace(std::begin(m_list), std::move(element));
            }

            void list_value::push_front(const list_value &other)
            {
                auto copied = copy(other);
                m_list.insert(std::begin(m_list),
                               std::make_move_iterator(std::begin(copied)), std::make_move_iterator(std::end(copied)));
            }

            void list_value::pop_back()
            {
                m_list.pop_back();
            }

            void list_value::pop_front()
            {
                m_list.pop_front();
            }

            instance &list_value::at(int_t index)
            {
                auto& instance_ptr = m_list.at(static_cast<unsigned>(index));
                return *instance_ptr;
            }

            instance &list_value::front()
            {
                auto& instance_ptr = m_list.front();
                return *instance_ptr;
            }

            instance &list_value::back()
            {
                auto& instance_ptr = m_list.back();
                return *instance_ptr;
            }

            void list_value::insert(int_t pos, element_t element)
            {
                const auto where = place(pos);
                m_list.emplace(where, std::move(element));
            }

            void list_value::insert(int_t pos, const list_value& other)
            {
                auto copied = copy(other);
                const auto where = place(pos);
                m_list.insert(where,
                               std::make_move_iterator(std::begin(copied)), std::make_move_iterator(std::end(copied)));
            }

            list_value::container_t list_value::copy(const list_value &other) const
            {
                container_t copied;
                append_with_copy_to(other, copied);
                return copied;
            }

            void list_value::erase(int_t pos, int_t count)
            {
                count = interpret_count(count, 1);
                const auto where = place(pos);
                const auto end = place(pos + count);
                m_list.erase(where, end);
            }

            int_t list_value::interpret_count(int_t value, int_t special_value) const
            {
                return value == k_special_value ? special_value : value;
            }

            list_value::container_t::const_iterator list_value::place(int_t pos) const
            {
                return std::next(cbegin(), pos);
            }

            int_t list_value::remove(const instance &value, int_t count)
            {
                return remove_impl(value, count, [](const auto i) { return i; });
            }

            int_t list_value::remove_last(const instance &value, int_t count)
            {
                return remove_impl(value, count,
                                   [&list = m_list](const auto i)
                                   {
                                       return list.size() - 1 - i;
                                   });
            }

            template <typename IndexCalculator>
            int_t list_value::remove_impl(const instance& value, int_t count, IndexCalculator&& indexCalculator)
            {
                int_t erased_counter{ 0 };
                count = interpret_count(count, m_list.size());
                auto i{ 0 };
                while(i < m_list.size() && count != 0)
                {
                    const auto check_index = indexCalculator(i);
                    if(m_list[check_index]->get_value_cref() != value.get_value_cref())
                    {
                        ++i;
                        continue;
                    }

                    erase(i);
                    --count;
                    ++erased_counter;
                }

                return erased_counter;
            }

            void list_value::clear()
            {
                m_list.clear();
            }

            void list_value::resize(int_t new_size)
            {
                m_list.resize(static_cast<unsigned>(new_size));
            }

            void list_value::sort()
            {
                const auto pred = [](const auto& lhs, const auto& rhs)
                {
                    return lhs->get_value_cref() < rhs->get_value_cref();
                };

                std::sort(begin(), end(), pred);
            }

            void list_value::reverse()
            {
                std::reverse(begin(), end());
            }

            int_t list_value::min() const
            {
                const auto min_it = std::min_element(cbegin(), cend());
                if(min_it == cend())
                {
                    return -1;
                }

                const auto index = std::distance(cbegin(), min_it);
                return static_cast<int_t>(index);
            }

            int_t list_value::max() const
            {
                const auto min_it = std::max_element(cbegin(), cend());
                if(min_it == cend())
                {
                    return -1;
                }

                const auto index = std::distance(cbegin(), min_it);
                return static_cast<int_t>(index);
            }

            list_value::container_t::iterator list_value::begin()
            {
                return std::begin(m_list);
            }

            list_value::container_t::iterator list_value::end()
            {
                return std::end(m_list);
            }

            list_value::container_t::const_iterator list_value::cbegin() const
            {
                return std::cbegin(m_list);
            }

            list_value::container_t::const_iterator list_value::cend() const
            {
                return std::cend(m_list);
            }

            list_value list_value::sublist(int_t pos, int_t count) const
            {
                count = interpret_count(count, m_list.size() - pos);
                const auto from = place(pos);
                const auto to = place(pos + count);
                container_t copied;
                std::transform(from, to,
                        std::back_inserter(copied),
                        [](const auto& element)
                               {
                                    return element->copy();
                               });

                return list_value{ std::move(copied) };
            }

            int_t list_value::size() const
            {
                return static_cast<int_t>(m_list.size());
            }

            bool list_value::empty() const
            {
                return m_list.empty();
            }

            int_t list_value::find(const instance &value, int_t pos) const
            {
                pos = interpret_count(pos, 0);
                const auto start = place(pos);
                const auto found = std::find_if(start, cend(),
                        [&value](const auto& element)
                        {
                            return element->get_value_cref() == value.get_value_cref();
                        });

                return found == cend() ? -1 : std::distance(cbegin(), found);
            }
        }
    }
}
