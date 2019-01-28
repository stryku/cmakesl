#pragma once

namespace cmsl
{
    namespace exec
    {
        class function_caller2;
        class identifiers_context;

        namespace inst
        {
            class instances_holder_interface;
        }

        struct expression_evaluation_context
        {
            function_caller2& function_caller;
            inst::instances_holder_interface& instances;
            identifiers_context& ids_context;
        };
    }
}
