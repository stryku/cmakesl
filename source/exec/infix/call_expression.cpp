#include "exec/infix/call_expression.hpp"

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
                return cmsl::string_view();
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
        }
    }
}
