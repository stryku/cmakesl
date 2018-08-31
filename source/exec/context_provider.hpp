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

        class context_provider
        {
        public:
            virtual ~context_provider() {}
            virtual const ast::ast_context& get_ast_ctx() const = 0;
            virtual execution_context& get_exec_ctx() = 0;
        };
    }
}