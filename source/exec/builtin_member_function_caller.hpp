#pragma once

#include "common/string.hpp"

#include <vector>

namespace cmsl
{
    namespace ast
    {
        class type;
    }

    namespace exec
    {
        namespace inst
        {
            class instance;
            class instances_holder;
        }

        // Class to call member functions on builtin types. e.g. int::to_string().
        class builtin_member_function_caller
        {
        public:
            explicit builtin_member_function_caller(inst::instances_holder& instances);

            inst::instance* call(inst::instance* class_instance,
                                 cmsl::string_view fun_name,
                                 std::vector<inst::instance*> parameters);

        private:
            inst::instance* call_push_back(inst::instance* class_instance, std::vector<inst::instance*> parameters);
            inst::instance* call_version_ctor(const ast::type& type, std::vector<inst::instance*> parameters);

        private:
            inst::instances_holder& m_instances;
        };
    }
}
