#pragma once

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

            // Todo: consider returning a reference
            inst::instance* call_member(inst::instance& instance, sema::builtin_function_kind function_kind, const params_t& params);

        private:
            inst::instance* int_operator_minus(inst::instance& instance, const params_t& params);
            inst::instance* int_operator_equal(inst::instance& instance, const params_t& params);

        private:
            inst::instances_holder_interface& m_instances;
        };
    }
}



