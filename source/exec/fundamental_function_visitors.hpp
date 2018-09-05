#pragma once

#include "common/type_traits.hpp"
#include "exec/instance/instance_value.hpp"

namespace cmsl
{
    namespace exec
    {
        namespace details
        {
            struct arithmetical_types_sink : public boost::static_visitor<inst::instance_value_t>
            {
                inst::instance_value_t operator()(bool) const
                {
                    // Should never be called
                    return 0;
                }
                inst::instance_value_t operator()(int) const
                {
                    // Should never be called
                    return 0;
                }
                inst::instance_value_t operator()(double) const
                {
                    // Should never be called
                    return 0;
                }
            };
        }

        struct size_visitor : public details::arithmetical_types_sink
        {
            template <typename ValueType>
            auto operator()(ValueType&& val) const -> decltype(static_cast<int>(val.size()))
            {
                return static_cast<int>(val.size());
            }

            int operator()(inst::generic_instance_value& val) const
            {
                return val.apply([](const auto& stored_value)
                                 {
                                     return stored_value.size();
                                 });
            }

            using details::arithmetical_types_sink::operator();
        };

        struct empty_visitor : public details::arithmetical_types_sink
        {
            template <typename ValueType>
            auto operator()(ValueType&& val) const -> decltype(val.empty())
            {
                return val.empty();
            }

            bool operator()(inst::generic_instance_value& val) const
            {
                return val.apply([](const auto& stored_value)
                                 {
                                     return stored_value.empty();
                                 });
            }

            using details::arithmetical_types_sink::operator();
        };
    }
}
