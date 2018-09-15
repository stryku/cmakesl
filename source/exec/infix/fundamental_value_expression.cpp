#include "exec/infix/fundamental_value_expression.hpp"
#include "exec/infix/infix_evaluation_context.hpp"
#include "exec/instance/instances_holder.hpp"
#include "exec/instance/instance.hpp"

namespace cmsl
{
    namespace exec
    {
        namespace infix
        {
            fundamental_value_expression::fundamental_value_expression(lexer::token::token token)
                : m_token{ token }
            {}

            bool_value_expression::bool_value_expression(lexer::token::token token)
                : fundamental_value_expression(token)

            {}

            inst::instance *bool_value_expression::evaluate(infix_evaluation_context &ctx) const
            {
                const auto val = m_token.str() == "true";
                return ctx.instances.create(val);
            }


            int_value_expression::int_value_expression(lexer::token::token token)
                : fundamental_value_expression(token)
            {}

            inst::instance* int_value_expression::evaluate(infix_evaluation_context& ctx) const
            {
                const auto val = std::stoll(m_token.str().to_string());
                return ctx.instances.create(static_cast<inst::int_t>(val));
            }


            double_value_expression::double_value_expression(lexer::token::token token)
                : fundamental_value_expression(token)
            {}

            inst::instance *double_value_expression::evaluate(infix_evaluation_context &ctx) const
            {
                const auto val = std::stod(m_token.str().to_string());
                return ctx.instances.create(val);
            }


            string_value_expression::string_value_expression(lexer::token::token token)
                : fundamental_value_expression(token)
            {}

            inst::instance *string_value_expression::evaluate(infix_evaluation_context &ctx) const
            {
                // string tokens are still surrounded by "". We need to remove them.
                const auto val = cmsl::string_view{
                        std::next(m_token.str().begin()),
                        m_token.str().size() - 2u // -2 to omit both ""
                };

                return ctx.instances.create(val.to_string());
            }
        }
    }
}
