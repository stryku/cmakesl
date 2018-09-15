#include "exec/infix/fundamental_value_expression.hpp"
#include "exec/infix/infix_evaluation_context.hpp"
#include "exec/instance/instances_holder.hpp"

namespace cmsl
{
    namespace exec
    {
        namespace infix
        {
            fundamental_value_expression::fundamental_value_expression(lexer::token::token token)
                : m_token{ token }
            {}

            inst::instance* fundamental_value_expression::evaluate(infix_evaluation_context &ctx) const
            {
                const auto val = std::stoll(m_token.str().to_string());
                return ctx.instances.create(static_cast<inst::int_t>(val));
            }
        }
    }
}
