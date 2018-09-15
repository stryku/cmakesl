#pragma once

namespace cmsl
{
    namespace ast
    {
        class ast_context;
    }

    namespace exec
    {
        class execution_context;

        namespace inst
        {
            class instances_holder;
        }

        namespace infix
        {
            struct infix_evaluation_context
            {
                const ast::ast_context& ast_ctx;
                ast::ast_context& exec_ctx;
                inst::instances_holder& instances;
            };
        }
    }
}
