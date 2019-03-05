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
            bool_operator_pipe_pipe,
            bool_operator_amp_amp,
            bool_to_string,

            int_ctor,
            int_ctor_bool,
            int_ctor_int,
            int_ctor_double,
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

            double_ctor,
            double_ctor_double,
            double_ctor_int,
            double_operator_plus,
            double_operator_minus,
            double_operator_star,
            double_operator_slash,
            double_operator_equal,
            double_operator_plus_equal,
            double_operator_minus_equal,
            double_operator_star_equal,
            double_operator_slash_equal,
            double_operator_less,
            double_operator_less_equal,
            double_operator_greater,
            double_operator_greater_equal,
            double_to_string,

            string_empty,
            string_size

        };
    }
}
