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
            struct conversion_visitor
            {
                typedef instance_value_t result_type;

                instance_value_t visit(const instance_value_t& from, const instance_value_t& to) const
                {
                    const auto from_val_type = get_inst_val_type(from);

                    switch(from_val_type)
                    {
                        case instance_value_type::bool_:
                            return convert_from_arithmetic(from.get_bool(), to);

                        case instance_value_type::int_:
                            return convert_from_arithmetic(from.get_int(), to);

                        case instance_value_type::double_:
                            return convert_from_arithmetic(from.get_double(), to);

                        case instance_value_type::string:
                            return convert_from_string(from.get_string_cref(), to);

                        case instance_value_type::generic:
                            return convert_from_generic(from.get_generic_cref(), to);
                    }
                }

            private:
                template <typename DesiredType>
                auto apply_generic_conversion(const generic_instance_value& from)
                {
                    DesiredType result;

                    auto transformer = [&result](const auto& from_val)
                    {
                        std::transform(std::cbegin(from_val), std::cend(from_val),
                                       std::back_inserter(result),
                                       [](const auto& instance)
                                       {
                                           return instance->copy();
                                       });
                    };

                    from.apply(transformer);

                    return result;
                }

                instance_value_t convert_from_generic(const generic_instance_value& from, const instance_value_t& to) const
                {
                    const auto to_val_type = get_inst_val_type(to);

                    if(to_val_type != instance_value_type::generic)
                    {
                        CMSL_UNREACHABLE("Applying not possible conversion");
                    }

                    const auto& generic_to = to.get_generic_cref();

                    generic_instance_value converted{ generic_to.get_type(), generic_to.get_value_type() };

                    converted.copy_value(from);

                    return converted;
                }

                instance_value_t convert_from_string(const std::string& from, const instance_value_t& to) const
                {
                    const auto to_val_type = get_inst_val_type(to);

                    if(to_val_type != instance_value_type::string)
                    {
                        CMSL_UNREACHABLE("Applying not possible conversion");
                    }

                    return from;
                }

                template <typename FromType>
                instance_value_t convert_from_arithmetic(FromType&& from, const instance_value_t& to) const
                {
                    const auto to_val_type = get_inst_val_type(to);

                    switch(to_val_type)
                    {
                        case instance_value_type::bool_:
                            return static_cast<bool>(from);

                        case instance_value_type::int_:
                            return static_cast<inst::int_t>(from);

                        case instance_value_type::double_:
                            return static_cast<double>(from);

                        default:
                            CMSL_UNREACHABLE("Applying not possible conversion");
                    }
                }
/*
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
                */
            };
        }
    }
}
