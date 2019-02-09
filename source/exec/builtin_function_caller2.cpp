#include "exec/builtin_function_caller2.hpp"
#include "sema/builtin_function_kind.hpp"
#include "exec/instance/instance.hpp"
#include "common/assert.hpp"


namespace cmsl
{
    namespace exec
    {
        inst::instance_value_t builtin_function_caller2::call_member(inst::instance& instance, sema::builtin_function_kind function_kind, const params_t& params)
        {
            switch (function_kind)
            {
                case sema::builtin_function_kind::int_operator_minus:
                {
                    return int_operator_minus(instance, params);
                }

                default:
                    CMSL_UNREACHABLE("Calling unimplemented member function");

            }

            CMSL_UNREACHABLE("Calling unknown member function");
            return inst::instance_value_t{};
        }

        inst::instance_value_t
        builtin_function_caller2::int_operator_minus(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            const auto lhs = instance.get_value_cref().get_int();
            const auto rhs = params[0]->get_value_cref().get_int();
            return lhs - rhs;
        }
    }
}



