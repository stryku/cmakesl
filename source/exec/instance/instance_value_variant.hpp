#pragma once

#include "exec/instance/int_alias.hpp"

#include <string>

namespace cmsl
{
    namespace exec
    {
        namespace inst
        {
            class instance_value_variant
            {
            public:
                enum class which_type
                {
                    bool_,
                    int_,
                    double_,
                    string
                };

                instance_value_variant(const instance_value_variant&) = default;
                instance_value_variant& operator=(const instance_value_variant&)  = default;
                instance_value_variant(instance_value_variant&&) = default;
                instance_value_variant& operator=(instance_value_variant&&)  = default;

                instance_value_variant()
                        : m_value{ false }
                {}

                instance_value_variant(bool val)
                        : m_value{ val }
                    , m_which{ which_type::bool_ }
                {}

                instance_value_variant(int_t val)
                        : m_value{ val }
                        , m_which{ which_type::int_ }
                {}

                instance_value_variant(double val)
                        : m_value{ val }
                        , m_which{ which_type::double_ }
                {}

                instance_value_variant(std::string val)
                        : m_value{ std::move(val) }
                        , m_which{ which_type::string }
                {}

                which_type which() const
                {
                    return m_which;
                }

                bool get_bool() const
                {
                    return m_value.m_bool;
                }

                int_t get_int() const
                {
                    return m_value.m_int;
                }

                double get_double() const
                {
                    return m_value.m_double;
                }

                const std::string& get_string_cref() const
                {
                    return m_value.m_string;
                }

            private:
                union
                {
                    bool m_bool;
                    int_t m_int;
                    double m_double;
                    std::string m_string;
                } m_value;

                which_type m_which;
            };
        }
    }
}
