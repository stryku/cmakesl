#pragma once

#include <memory>
#include <vector>

namespace cmsl
{
    namespace sema
    {
        enum class builtin_function_kind;
    }

    namespace exec
    {
        namespace inst
        {
            class instance;
            class instances_holder_interface;
        }

        class builtin_function_caller2
        {
        public:
            using params_t = std::vector<inst::instance*>;

            explicit builtin_function_caller2(inst::instances_holder_interface& instances);

            std::unique_ptr<inst::instance> call_member(inst::instance& instance, sema::builtin_function_kind function_kind, const params_t& params);

        private:
            inst::instance* bool_ctor(inst::instance& instance, const params_t& params);
            inst::instance* bool_ctor_bool(inst::instance& instance, const params_t& params);
            inst::instance* bool_ctor_int(inst::instance& instance, const params_t& params);
            inst::instance* bool_operator_equal(inst::instance& instance, const params_t& params);

            inst::instance* int_ctor(inst::instance& instance, const params_t& params);
            inst::instance* int_ctor_bool(inst::instance& instance, const params_t& params);
            inst::instance* int_ctor_int(inst::instance& instance, const params_t& params);
            inst::instance* int_operator_plus(inst::instance& instance, const params_t& params);
            inst::instance* int_operator_minus(inst::instance& instance, const params_t& params);
            inst::instance* int_operator_star(inst::instance& instance, const params_t& params);
            inst::instance* int_operator_slash(inst::instance& instance, const params_t& params);
            inst::instance* int_operator_equal(inst::instance& instance, const params_t& params);
            inst::instance* int_operator_plus_equal(inst::instance& instance, const params_t& params);
            inst::instance* int_operator_minus_equal(inst::instance& instance, const params_t& params);
            inst::instance* int_operator_star_equal(inst::instance& instance, const params_t& params);
            inst::instance* int_operator_slash_equal(inst::instance& instance, const params_t& params);
            inst::instance* int_operator_less(inst::instance& instance, const params_t& params);
            inst::instance* int_operator_less_equal(inst::instance& instance, const params_t& params);
            inst::instance* int_operator_greater(inst::instance& instance, const params_t& params);
            inst::instance* int_operator_greater_equal(inst::instance& instance, const params_t& params);
            inst::instance* int_operator_equal_equal(inst::instance& instance, const params_t& params);

        private:
            inst::instances_holder_interface& m_instances;
        };
    }
}



