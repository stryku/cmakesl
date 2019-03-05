#include "exec/builtin_function_caller2.hpp"
#include "sema/builtin_function_kind.hpp"
#include "exec/instance/instance.hpp"
#include "common/assert.hpp"
#include "builtin_function_caller2.hpp"
#include "exec/instance/instances_holder.hpp"

#define CASE_BUILTIN_FUNCTION_CALL(function) \
case sema::builtin_function_kind::function: \
{ \
    result = function(instance, params); \
} break

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
                // bool
                CASE_BUILTIN_FUNCTION_CALL(bool_ctor);
                CASE_BUILTIN_FUNCTION_CALL(bool_ctor_bool);
                CASE_BUILTIN_FUNCTION_CALL(bool_ctor_int);
                CASE_BUILTIN_FUNCTION_CALL(bool_operator_equal);

                // int
                CASE_BUILTIN_FUNCTION_CALL(int_ctor);
                CASE_BUILTIN_FUNCTION_CALL(int_ctor_bool);
                CASE_BUILTIN_FUNCTION_CALL(int_ctor_int);
                CASE_BUILTIN_FUNCTION_CALL(int_operator_plus);
                CASE_BUILTIN_FUNCTION_CALL(int_operator_minus);
                CASE_BUILTIN_FUNCTION_CALL(int_operator_equal);
                CASE_BUILTIN_FUNCTION_CALL(int_operator_plus_equal);

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

        inst::instance *
        builtin_function_caller2::bool_ctor(inst::instance &instance, const builtin_function_caller2::params_t&)
        {
            instance.get_value_ref().set_bool(false);
            return &instance;
        }

        inst::instance *
        builtin_function_caller2::bool_ctor_bool(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            const auto param = params[0]->get_value_cref()
                                        .get_bool();
            instance.get_value_ref().set_bool(param);
            return &instance;
        }

        inst::instance *
        builtin_function_caller2::bool_ctor_int(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            const auto param = params[0]->get_value_cref()
                                               .get_int();
            instance.get_value_ref().set_bool(static_cast<bool>(param));
            return &instance;
        }

        inst::instance *
        builtin_function_caller2::bool_operator_equal(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            const auto rhs = params[0]->get_value_cref()
                                      .get_bool();
            instance.get_value_ref() = rhs;
            return m_instances.create2_reference(instance);
        }

        inst::instance *
        builtin_function_caller2::int_ctor(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            instance.get_value_ref().set_int(inst::int_t{ 0 });
            return &instance;
        }

        inst::instance *
        builtin_function_caller2::int_ctor_int(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            const auto rhs = params[0]->get_value_cref()
                                      .get_int();
            instance.get_value_ref() = rhs;
            return &instance;
        }

        inst::instance *
        builtin_function_caller2::int_operator_plus(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            const auto lhs = instance.get_value_cref()
                                     .get_int();
            const auto rhs = params[0]->get_value_cref()
                                      .get_int();
            return m_instances.create2(lhs + rhs);
        }

        inst::instance *
        builtin_function_caller2::int_operator_plus_equal(inst::instance &instance, const builtin_function_caller2::params_t &params)
        {
            const auto lhs = instance.get_value_cref()
                                     .get_int();
            const auto rhs = params[0]->get_value_cref()
                                      .get_int();

            instance.get_value_ref().set_int(lhs + rhs);
            return m_instances.create2_reference(instance);
        }
    }
}