#pragma once

#include "exec/builtin/evaluatable_member_function.hpp"

#include <cstddef>

namespace cmsl
{
    namespace exec
    {
        namespace builtin
        {
            class size_member_function : public evaluatable_member_function
            {
            public:
                explicit size_member_function(inst::instance& class_instance, inst::instances_holder_interface& instances);

                inst::instance* evaluate() override;

            private:
                size_t get_size();
            };

            class empty_member_function : public evaluatable_member_function
            {
            public:
                explicit empty_member_function(inst::instance& class_instance, inst::instances_holder_interface& instances);

                inst::instance* evaluate() override;

            private:
                bool get_empty();
            };

            class push_back_member_function : public evaluatable_member_function
            {
            public:
                explicit push_back_member_function(inst::instance& class_instance, inst::instances_holder_interface& instances, const inst::instance& parameter);

                inst::instance* evaluate() override;

            private:
                const inst::instance& m_parameter;
            };
        }
    }
}
