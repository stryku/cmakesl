#pragma once

#include "exec/instance/instance_value.hpp"

#include <vector>

namespace cmsl
{
    namespace sema
    {
        enum class builtin_function_kind;
    }

    namespace exec
    {
        class builtin_function_caller2
        {
        public:
            using params_t = std::vector<inst::instance*>;

            inst::instance_value_t call_member(inst::instance& instance, sema::builtin_function_kind function_kind, const params_t& params);

        private:
            inst::instance_value_t int_operator_minus(inst::instance& instance, const params_t& params);
        };
    }
}



