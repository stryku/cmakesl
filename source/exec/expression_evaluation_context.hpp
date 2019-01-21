#pragma once

namespace cmsl
{
    namespace exec
    {
        class context_provider;
        class value_operations_factory;

        namespace inst
        {
            class instances_holder_interface;
        }

        struct expression_evaluation_context
        {
            context_provider& ctx_provider;
            inst::instances_holder_interface& instances;
            value_operations_factory& operations_factory;
        };
    }
}
