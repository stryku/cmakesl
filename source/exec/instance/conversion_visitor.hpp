#pragma once

#include "common/assert.hpp"
#include "exec/instance/instance_value.hpp"

#include <boost/variant.hpp>

#include <type_traits>

namespace cmsl
{
    namespace exec
    {
        namespace inst
        {
            struct conversion_visitor : boost::static_visitor<instance_value_t>
            {
                template <typename T>
                constexpr static auto is_decayed_arithmetic_v = std::is_arithmetic<std::decay_t<T>>::value;

                template <typename T>
                using enable_arithmetic = std::enable_if_t<is_decayed_arithmetic_v<T>>;

                // todo use std::negation
                template <typename T>
                using disable_arithmetic = std::enable_if_t<!is_decayed_arithmetic_v<T>>;

                template <typename T, typename T2>
                using disable_if_both_arithmetic = std::enable_if_t<!is_decayed_arithmetic_v<T> || !is_decayed_arithmetic_v<T2>>;

                template <typename T, typename T2>
                using enable_if_both_arithmetic = std::enable_if_t<is_decayed_arithmetic_v<T> && is_decayed_arithmetic_v<T2>>;

                template <typename ValueType>
                instance_value_t operator()(ValueType &&val, ValueType &&) const
                {
                    return val;
                }

                template<typename ValueToConvert,
                         typename DesiredType,
                         typename DecayedDesiredType = std::decay_t<DesiredType>,
                         typename = enable_if_both_arithmetic<ValueToConvert, DesiredType>>
                instance_value_t operator()(ValueToConvert &&val, DesiredType &&) const
                {
                    return static_cast<DecayedDesiredType>(val);
                }

                template <typename T, typename T2,
                         typename = disable_if_both_arithmetic<T, T2>>
                instance_value_t operator()(T&&, T2&&) const
                {
                    CMSL_UNREACHABLE("Applying not possible conversion");
                    return 1;
                };
            };
        }
    }
}
