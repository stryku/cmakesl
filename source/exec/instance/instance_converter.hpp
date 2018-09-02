#pragma once

#include "exec/instance/instance_value.hpp"

#include <utility>

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

            class instance_converter
            {
            public:
                explicit instance_converter(instances_holder& instances);

                instance* convert_to_type(instance* from, const ast::type& desired_type);
                instance* convert_to_type(instance_value_t from, const ast::type& desired_type);

                std::pair<instance*, instance*> prepare_for_operator(instance* lhs, instance* rhs);

            private:
                instances_holder& m_instances;
            };
        }
    }
}
