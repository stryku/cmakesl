#pragma once

#include "common/string.hpp"
#include "exec/execution_context.hpp"

#include <boost/optional.hpp>

#include <stack>

namespace cmsl
{
    namespace ast
    {
        class ast_node;
        class function_node;
        class ast_context;
        class block_node;
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

            void return_from_function(int value);
            execution_context& get_exec_ctx();
            const ast::ast_context& get_ast_ctx() const;
            void function_call(const ast::function_node& fun, std::vector<inst::instance*> parameters);
            void member_function_call(const ast::function_node& fun, std::vector<inst::instance*> parameters, inst::instance* class_instance);
            int get_function_return_value() const;
            void block(const ast::block_node& block_node);

        private:
            void return_from_function();
            bool execute_function_expression(ast::ast_node& expr);
            void execute_function_call(const ast::function_node& fun, std::vector<inst::instance*> parameters);
            void execute_block(const ast::block_node& block_node);

            bool returning_from_function() const;

        private:
            struct callstack_frame
            {
                const ast::function_node* fun;
                execution_context exec_ctx;
            };

            const ast::ast_context* m_ast_context;
            boost::optional<int> m_function_return_value;
            std::stack<callstack_frame> m_callstack;
        };
    }
}
