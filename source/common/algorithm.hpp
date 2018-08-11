#pragma once

#include <algorithm>
#include <utility>

namespace cmsl
{
    namespace details
    {
        template <size_t N>
        struct contains_helper;

        template <size_t N>
        struct contains_helper : contains_helper<N - 1>
        {};

        template <>
        struct contains_helper<0>
        {};

        template <typename Container, typename Value>
        auto contains_impl(Container&& container, Value&& value, contains_helper<1>) -> decltype(std::declval<Container>().find(value) != std::declval<Container>().end())
        {
            return container.find(value) != container.cend();
        }

        template <typename Container, typename Value>
        bool contains_impl(Container&& container, Value&& value, contains_helper<0>)
        {
            return std::find(std::cbegin(container), std::cend(container), value) != std::cend(container);
        }
    }

    template <typename Container, typename Value>
    bool contains(Container&& container, Value&& value)
    {
        return details::contains_impl(std::forward<Container>(container),
                                      std::forward<Value>(value),
                                      details::contains_helper<1>{});
    }
}
