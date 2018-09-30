#include "exec/infix/user_function_call_expression.hpp"

#include "ast/ast_context.hpp"
#include "exec/context_provider.hpp"
#include "exec/function_caller.hpp"

#include <algorithm>
#include "exec/infix/infix_expression_visitor.hpp"

namespace cmsl
{
    namespace exec
    {
        namespace infix
        {
            function_call_expression::function_call_expression(function_caller& fun_caller, lexer::token::token name, std::vector<std::unique_ptr<infix_expression>> parameter_expressions)
                : call_expression{ fun_caller, name, std::move(parameter_expressions) }
            {}

            inst::instance *function_call_expression::evaluate(infix_evaluation_context &ctx) const
            {
                auto evaluated_params =  evaluate_params(ctx);
                const auto& ast_ctx = ctx.m_ctx_provider.get_ast_ctx();
                const auto fun = ast_ctx.find_function(get_name());
                return get_caller().call(*fun, evaluated_params);
            }

            void function_call_expression::visit(infix_expression_visitor &visitor) const
            {
                visitor.visit(*this);
            }
        }
    }
}
