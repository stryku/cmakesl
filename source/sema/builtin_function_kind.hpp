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
            bool_to_string,

            int_ctor_bool,
            int_to_string,
            int_operator_plus,
            int_operator_minus,
            int_operator_equal,

            double_to_string,

            string_empty,
            string_size

        };
    }
}
