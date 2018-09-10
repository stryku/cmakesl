#pragma once

#include "exec/context_provider.hpp"
#include "exec/execution_context.hpp"

#include <boost/optional.hpp>

#include <stack>

namespace cmsl
{
    namespace ast
    {
        class ast_node;
        class user_function_node;
        class ast_context;
        class block_node;
    }

    namespace exec
    {
        namespace inst
        {
            class instance;
        }

        class execution : public context_provider
        {
        public:
            execution_context& get_exec_ctx() override;
            const ast::ast_context& get_ast_ctx() const override;

            void function_call(const ast::user_function_node& fun, std::vector<inst::instance*> parameters);
            void member_function_call(const ast::user_function_node& fun, std::vector<inst::instance*> parameters, inst::instance* class_instance);

            void block(const ast::block_node& block_node);

            void return_from_function(std::unique_ptr<inst::instance> value);
            std::unique_ptr<inst::instance> get_function_return_value() const;

        private:
            void return_from_function();
            bool execute_function_expression(ast::ast_node& expr); // todo rename. Block also uses this
            void execute_function_call(const ast::user_function_node& fun, std::vector<inst::instance*> parameters);
            void execute_block(const ast::block_node& block_node);

            bool returning_from_function() const;

            const ast::type& get_current_function_return_type() const;

        private:
            struct callstack_frame
            {
                const ast::user_function_node* fun;
                execution_context exec_ctx;
            };

            const ast::ast_context* m_ast_context;
            std::unique_ptr<inst::instance> m_function_return_value;
            std::stack<callstack_frame> m_callstack;
        };
    }
}
