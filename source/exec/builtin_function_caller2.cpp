#include "exec/builtin_function_caller2.hpp"
#include "sema/builtin_function_kind.hpp"
#include "exec/instance/instance.hpp"
#include "common/assert.hpp"
#include "builtin_function_caller2.hpp"
#include "exec/instance/instances_holder.hpp"

namespace cmsl
{
    namespace exec
    {
        builtin_function_caller2::builtin_function_caller2(inst::instances_holder_interface &instances)
            : m_instances{ instances }
        {}

        inst::instance*
        builtin_function_caller2::call_member(inst::instance &instance, sema::builtin_function_kind function_kind, const params_t &params)
        {
            switch (function_kind)
            {
                case sema::builtin_function_kind::int_operator_minus:
                {
                    return int_operator_minus(instance, params);
                }
                case sema::builtin_function_kind::int_operator_equal:
                {
                    return int_operator_equal(instance, params);
                }

                default:
                    CMSL_UNREACHABLE("Calling unimplemented member function");

            }

            CMSL_UNREACHABLE("Calling unknown member function");
            return nullptr;
        }

        inst::instance*
        builtin_function_caller2::int_operator_minus(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            const auto lhs = instance.get_value_cref()
                                     .get_int();
            const auto rhs = params[0]->get_value_cref()
                                      .get_int();
            return m_instances.create2(lhs - rhs);
        }

        inst::instance*
        builtin_function_caller2::int_operator_equal(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            const auto rhs = params[0]->get_value_cref()
                                      .get_int();
            instance.get_value_ref() = rhs;
            return m_instances.create2_reference(instance);
        }
    }
}