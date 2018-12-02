#pragma once

#include "exec/builtin/evaluatable_function.hpp"

namespace cmsl
{
    namespace exec
    {
        namespace builtin
        {
            class evaluatable_member_function : public evaluatable_function
            {
            public:
                explicit evaluatable_member_function(inst::instance& class_instance, inst::instances_holder& instances)
                    : evaluatable_function{ instances }
                    , m_class_instance{ class_instance }
                {}

                virtual ~evaluatable_member_function() = default;

            protected:
                inst::instance& m_class_instance;
            };
        }
    }
}
