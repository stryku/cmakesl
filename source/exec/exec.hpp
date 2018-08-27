#pragma once

#include "common/string.hpp"
#include "exec/execution_context.hpp"

#include <stack>

namespace cmsl
{
    namespace ast
    {
        class ast_node;
        class function_node;
        class ast_context;
    }

    namespace exec
    {
        namespace inst
        {
            class instance;
        }

        class executor
        {
        public:
            int execute(cmsl::string_view source);

            void set_function_return_value(int value);
            execution_context& get_exec_ctx();
            const ast::ast_context& get_ast_ctx() const;
            void function_call(const ast::function_node& fun, std::vector<inst::instance*> parameters);
            void member_function_call(const ast::function_node& fun, std::vector<inst::instance*> parameters, inst::instance* class_instance);
            int get_function_return_value() const;

        private:
            void return_from_function();
            bool execute_function_expression(ast::ast_node& expr);
            void execute_function_call(const ast::function_node& fun, std::vector<inst::instance*> parameters);

        private:
            struct callstack_frame
            {
                const ast::function_node* fun;
                execution_context exec_ctx;
            };

            const ast::ast_context* m_ast_context;
            int m_function_return_value;
            std::stack<callstack_frame> m_callstack;
        };
    }
}
