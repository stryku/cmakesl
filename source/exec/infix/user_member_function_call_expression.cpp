#include "exec/infix/user_member_function_call_expression.hpp"

#include "exec/instance/instance.hpp"
#include "exec/function_caller.hpp"
#include "exec/infix/infix_expression_visitor.hpp"

namespace cmsl
{
    namespace exec
    {
        namespace infix
        {
            user_member_function_call_expression::user_member_function_call_expression(function_caller& fun_caller,
                                                                             std::unique_ptr<infix_expression> lhs,
                                                                             lexer::token::token name,
                                                                             user_member_function_call_expression::params_t params)
                :call_expression{fun_caller, name, std::move(params) }
                , m_lhs{ std::move(lhs) }
            {}

            inst::instance *user_member_function_call_expression::evaluate(infix_evaluation_context &ctx) const
            {
                auto evaluated_params = evaluate_params(ctx);
                auto lhs_instance = m_lhs->evaluate(ctx);
                auto fun = lhs_instance->get_function(get_name());
                return get_caller().call_member(*lhs_instance, *fun, std::move(evaluated_params));
            }

            void user_member_function_call_expression::visit(infix_expression_visitor &visitor) const
            {
                visitor.visit(*this);
            }
        }
    }
}
