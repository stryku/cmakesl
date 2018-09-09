#pragma once

#include "common/string.hpp"

#include <vector>

namespace cmsl
{
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
            inst::instances_holder& m_instances;
        };
    }
}
