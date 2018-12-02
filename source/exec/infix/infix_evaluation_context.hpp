#pragma once

namespace cmsl
{
    namespace exec
    {
        class context_provider;

        namespace inst
        {
            class instances_holder_interface;
        }

        namespace infix
        {
            struct infix_evaluation_context
            {
                context_provider& m_ctx_provider;
                inst::instances_holder_interface& instances;
            };
        }
    }
}
