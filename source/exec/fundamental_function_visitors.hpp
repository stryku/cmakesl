#pragma once

#include "common/type_traits.hpp"
#include "exec/instance/instance_value.hpp"

namespace cmsl
{
    namespace exec
    {
        struct size_visitor : public boost::static_visitor<inst::instance_value_t>
        {
            template <typename ValueType>
            auto operator()(ValueType&& val) const -> decltype(static_cast<int>(val.size()))
            {
                return static_cast<int>(val.size());
            }

            template <typename T,
                      typename = cmsl::enable_if_not_container<T>>
            inst::instance_value_t operator()(T&&) const
            {
                // Should never be called
                return 0;
            }
        };
    }
}
