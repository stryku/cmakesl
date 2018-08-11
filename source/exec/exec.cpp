#include "exec/exec.hpp"

#include "ast/ast_node.hpp"
#include "lexer/lexer.hpp"
#include "errors/errors_observer.hpp"
#include "ast/ast_builder.hpp"
#include "ast/expr/expression.hpp"
#include "ast/builtin_ast_context.hpp"
#include "ast/block_expression.hpp"

#include <iostream>

namespace cmsl
{
    namespace exec
    {
        int executor::execute(cmsl::string_view source)
        {
            errors::errors_observer err_observer;
            lexer::lexer lex{ err_observer , source };
            const auto tokens = lex.lex();

            ast::ast_builder builder;
            ast::builtin_ast_context ctx;
            auto global_ast_ctx = builder.build(ctx, err_observer, tokens);
            auto main_function = global_ast_ctx->find_function("main");
            function_call(*main_function);
            const auto main_result = m_function_return_value;
            return main_result;
            return 0;
        }

        void executor::function_call(ast::function& fun)
        {
            m_call_stack.push(&fun);
            auto& fun_body = fun.get_body();
            const auto expressions = fun_body.get_expressions();

            for (auto expr : expressions)
            {
                const auto returned_from_function = execute_function_expression(*expr);
                if (returned_from_function)
                {
                    return;
                }
            }
        }

        bool executor::execute_function_expression(ast::ast_node& expr)
        {
            switch (expr.get_type())
            {
                case ast::expression_type::ret:
                {
                    expr.execute(*this);
                    return_from_function();
                    return true;
                }
                break;
            }

            return false;
        }

        void executor::set_function_return_value(int value)
        {
            m_function_return_value = value;
        }

        void executor::return_from_function()
        {
            m_call_stack.pop();
        }
    }
}
