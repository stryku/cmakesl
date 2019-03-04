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

        std::unique_ptr<inst::instance>
        builtin_function_caller2::call_member(inst::instance &instance, sema::builtin_function_kind function_kind, const params_t &params)
        {
            inst::instance* result{ nullptr };

            switch (function_kind)
            {
                case sema::builtin_function_kind::int_ctor_bool:
                {
                    result = int_ctor_bool(instance, params);
                } break;
                case sema::builtin_function_kind::int_operator_minus:
                {
                    result = int_operator_minus(instance, params);
                } break;
                case sema::builtin_function_kind::int_operator_equal:
                {
                    result = int_operator_equal(instance, params);
                } break;

                default:
                    CMSL_UNREACHABLE("Calling unimplemented member function");
                    return nullptr;

            }

            return m_instances.gather_ownership(result);
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

        inst::instance *
        builtin_function_caller2::int_ctor_bool(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            const auto param = params[0]->get_value_cref()
                                        .get_bool();
            instance.get_value_ref().set_int(static_cast<inst::int_t>(param));
            return &instance;
        }
    }
}