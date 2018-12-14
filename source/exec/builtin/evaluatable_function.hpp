#pragma once

namespace cmsl
{
    namespace exec
    {
        namespace inst
        {
            class instance;
            class instances_holder_interface;
        }

        namespace builtin
        {
            class evaluatable_function
            {
            public:
                explicit evaluatable_function(inst::instances_holder_interface& instances)
                    : m_instances{ instances }
                {}

                virtual ~evaluatable_function() = default;

                virtual inst::instance* evaluate() = 0;

            protected:
                inst::instances_holder_interface& m_instances;
            };
        }
    }
}
