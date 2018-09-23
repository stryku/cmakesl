#include "exec/infix/member_function_call_expression.hpp"

#include "exec/instance/instance.hpp"
#include "exec/function_caller.hpp"

namespace cmsl
{
    namespace exec
    {
        namespace infix
        {
            member_function_call_expression::member_function_call_expression(function_caller& fun_caller,
                                                                             std::unique_ptr<infix_expression> lhs,
                                                                             lexer::token::token name,
                                                                             member_function_call_expression::params_t params)
                :call_expression{fun_caller, name, std::move(params) }
                , m_lhs{ std::move(lhs) }
            {}

            inst::instance *member_function_call_expression::evaluate(infix_evaluation_context &ctx) const
            {
                auto evaluated_params = evaluate_params(ctx);
                auto lhs_instance = m_lhs->evaluate(ctx);
                auto fun = lhs_instance->get_function(get_name());
                return get_caller().call_member(*lhs_instance, *fun, std::move(evaluated_params));
            }
        }
    }
}
