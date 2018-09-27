#include "exec/infix/fundamental_value_expression.hpp"
#include "exec/infix/infix_evaluation_context.hpp"
#include "exec/instance/instances_holder.hpp"
#include "exec/instance/instance.hpp"
#include "exec/infix/infix_expression_visitor.hpp"

namespace cmsl
{
    namespace exec
    {
        namespace infix
        {
            fundamental_value_expression::fundamental_value_expression(lexer::token::token token)
                : m_token{ token }
            {}

            lexer::token::token fundamental_value_expression::get_token() const
            {
                return m_token;
            }

            bool_value_expression::bool_value_expression(lexer::token::token token)
                : fundamental_value_expression(token)

            {}

            inst::instance *bool_value_expression::evaluate(infix_evaluation_context &ctx) const
            {
                const auto val = m_token.str() == "true";
                return ctx.instances.create(val);
            }

            void bool_value_expression::visit(infix_expression_visitor &visitor) const
            {
                return visitor.visit(*this);
            }


            int_value_expression::int_value_expression(lexer::token::token token)
                : fundamental_value_expression(token)
            {}

            inst::instance* int_value_expression::evaluate(infix_evaluation_context& ctx) const
            {
                const auto val = std::stoll(m_token.str().to_string());
                return ctx.instances.create(static_cast<inst::int_t>(val));
            }

            void int_value_expression::visit(infix_expression_visitor &visitor) const
            {
                return visitor.visit(*this);
            }


            double_value_expression::double_value_expression(lexer::token::token token)
                : fundamental_value_expression(token)
            {}

            inst::instance *double_value_expression::evaluate(infix_evaluation_context &ctx) const
            {
                const auto val = std::stod(m_token.str().to_string());
                return ctx.instances.create(val);
            }

            void double_value_expression::visit(infix_expression_visitor &visitor) const
            {
                return visitor.visit(*this);
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

            void string_value_expression::visit(infix_expression_visitor &visitor) const
            {
                return visitor.visit(*this);
            }


            std::unique_ptr<infix_expression> make_fundamental_expression(lexer::token::token token)
            {
                switch(token.get_type())
                {

                    case lexer::token::token_type::integer:
                        return std::make_unique<int_value_expression>(token);

                    case lexer::token::token_type::double_:
                        return std::make_unique<double_value_expression>(token);

                    case lexer::token::token_type::string:
                        return std::make_unique<string_value_expression>(token);

                    case lexer::token::token_type::kw_true:
                    case lexer::token::token_type::kw_false:
                        return std::make_unique<bool_value_expression>(token);

                    default:
                        return nullptr;
                }
            }
        }
    }
}
