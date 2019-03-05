#pragma once

namespace cmsl
{
    namespace sema
    {
        enum class builtin_function_kind
        {
            bool_ctor,
            bool_ctor_bool,
            bool_ctor_int,
            bool_operator_equal,
            bool_to_string,

            int_ctor,
            int_ctor_bool,
            int_ctor_int,
            int_to_string,
            int_operator_plus,
            int_operator_minus,
            int_operator_star,
            int_operator_slash,
            int_operator_equal,
            int_operator_plus_equal,
            int_operator_minus_equal,
            int_operator_star_equal,
            int_operator_slash_equal,
            int_operator_less,
            int_operator_less_equal,
            int_operator_greater,
            int_operator_greater_equal,
            int_operator_equal_equal,

            double_to_string,

            string_empty,
            string_size

        };
    }
}
