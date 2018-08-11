#pragma once

#include "common/string.hpp"
#include "exec/execution_context.hpp"

#include <stack>

namespace cmsl
{
    namespace ast
    {
        class ast_node;
        class function;
    }

    namespace exec
    {
        class executor
        {
        public:
            int execute(cmsl::string_view source);

            void set_function_return_value(int value);
            execution_context& get_ctx();

        private:
            void function_call(ast::function& fun);
            void return_from_function();
            bool execute_function_expression(ast::ast_node& expr);

        private:
            execution_context m_exec_ctx;
            int m_function_return_value;
            std::stack<ast::function*> m_call_stack;
        };
    }
}
