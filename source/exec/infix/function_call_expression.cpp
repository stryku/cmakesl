#include "exec/infix/function_call_expression.hpp"

#include "ast/ast_context.hpp"
#include "exec/context_provider.hpp"
#include "exec/function_caller.hpp"

#include <algorithm>

namespace cmsl
{
    namespace exec
    {
        namespace infix
        {
            function_call_expression::function_call_expression(function_caller& fun_caller, lexer::token::token name, std::vector<std::unique_ptr<infix_expression>> parameter_expressions)
                : m_fun_caller{ fun_caller }
                , m_name{ name }
                , m_parameter_expressions{ std::move( parameter_expressions) }
            {}

            inst::instance *function_call_expression::evaluate(infix_evaluation_context &ctx) const
            {
                std::vector<inst::instance*> evaluated_params;

                std::transform(std::cbegin(m_parameter_expressions), std::cend(m_parameter_expressions),
                              std::back_inserter(evaluated_params),
                              [&ctx](const auto& param_expr)
                              {
                                  return param_expr->evaluate(ctx);
                              });

                const auto& ast_ctx = ctx.m_ctx_provider.get_ast_ctx();
                const auto fun = ast_ctx.find_function(m_name.str());
                return m_fun_caller.call(*fun, evaluated_params);
            }
        }
    }
}
