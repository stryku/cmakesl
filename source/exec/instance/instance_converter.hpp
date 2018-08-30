#pragma once

#include <utility>

namespace cmsl
{
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

                std::pair<instance*, instance*> prepare_for_operator(instance* lhs, instance* rhs);

            private:
                instances_holder& m_instances;
            };
        }
    }
}
