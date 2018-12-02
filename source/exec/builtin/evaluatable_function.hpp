#pragma once

namespace cmsl
{
    namespace exec
    {
        namespace inst
        {
            class instance;
            class instances_holder;
        }

        namespace builtin
        {
            class evaluatable_function
            {
            public:
                explicit evaluatable_function(inst::instances_holder& instances)
                    : m_instances{ instances }
                {}

                virtual ~evaluatable_function() = default;

                virtual inst::instance* evaluate() = 0;

            protected:
                inst::instances_holder& m_instances;
            };
        }
    }
}
