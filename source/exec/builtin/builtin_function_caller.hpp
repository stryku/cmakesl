#pragma once

#include "ast/builtin_function_kind.hpp"
#include "common/string.hpp"

#include <vector>

namespace cmsl
{
    namespace ast
    {
        class type;
        class builtin_function;
    }

    namespace facade
    {
        class cmake_facade;
    }

    namespace exec
    {
        namespace inst
        {
            class instance;
            class instances_holder;
        }

        namespace builtin
        {
            // Class to call builtin functions (also on builtin types). e.g. int::to_string().
            class builtin_function_caller
            {
            private:
                using ff_kind_t = ast::builtin_function_kind;

            public:
                explicit builtin_function_caller(inst::instances_holder &instances, facade::cmake_facade& cmake_facade);

                inst::instance *call_member_function(inst::instance *class_instance,
                                                     cmsl::string_view fun_name,
                                                     const std::vector<inst::instance *> &parameters);

                inst::instance *call_member_function(inst::instance *class_instance,
                                                     const ast::builtin_function &fun,
                                                     const std::vector<inst::instance *> &parameters);

                inst::instance *call(ff_kind_t fun, std::vector<inst::instance *> parameters);

            private:
                inst::instance *
                call_push_back(inst::instance *class_instance, std::vector<inst::instance *> parameters);

                inst::instance *call_version_ctor(const ast::type &type, std::vector<inst::instance *> parameters);

                inst::instance *call_cmake_minimum_required(std::vector<inst::instance *> parameters);

            private:
                inst::instances_holder &m_instances;
                facade::cmake_facade& m_facade;
            };
        }
    }
}
