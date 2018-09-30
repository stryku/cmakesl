#include "exec/infix/call_expression.hpp"
#include "exec/infix/infix_evaluation_context.hpp"
#include "exec/context_provider.hpp"
#include "ast/ast_context.hpp"
#include "common/assert.hpp"

#include <algorithm>

namespace cmsl
{
    namespace exec
    {
        namespace infix
        {
            call_expression::call_expression(function_caller& fun_caller, lexer::token::token name, params_t parameter_expressions)
                : m_fun_caller{ fun_caller }
                , m_name{ name }
                , m_parameter_expressions{ std::move( parameter_expressions) }
            {}

            function_caller &call_expression::get_caller() const
            {
                return m_fun_caller;
            }

            cmsl::string_view call_expression::get_name() const
            {
                return m_name.str();
            }

            std::vector<inst::instance *> call_expression::evaluate_params(infix_evaluation_context &ctx) const
            {
                std::vector<inst::instance*> evaluated_params;

                std::transform(std::cbegin(m_parameter_expressions), std::cend(m_parameter_expressions),
                               std::back_inserter(evaluated_params),
                               [&ctx](const auto& param_expr)
                               {
                                   return param_expr->evaluate(ctx);
                               });

                return evaluated_params;
            }

            const ast::function &call_expression::get_function(infix_evaluation_context& ctx) const
            {
                const auto& ast_ctx =  ctx.m_ctx_provider.get_ast_ctx();
                auto fun = ast_ctx.find_function(get_name());
                if(fun)
                {
                    return *fun;
                }

                // Try to find type. If get_name() == some type name, that means we want to call a ctor.
                auto type = ast_ctx.find_type(get_name());
                if(type)
                {
                    return *type->get_function(get_name());
                }

                CMSL_UNREACHABLE("Function no found. This issue should be found during sema check.");
            }
        }
    }
}
