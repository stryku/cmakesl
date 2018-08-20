#pragma once

#include <boost/functional/hash.hpp>

#include <experimental/string_view>

#include <unordered_map>

namespace cmsl
{
    using string_view = std::experimental::string_view;

    namespace details
    {
        struct hasher
        {
            auto operator()(cmsl::string_view str) const
            {
                return boost::hash_range(str.begin(), str.end());
            }
        };
    }

    template <typename Value>
    using string_view_map = std::unordered_map<string_view, Value, details::hasher>;
}
