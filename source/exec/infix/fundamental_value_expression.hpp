#pragma once

#include "exec/infix/infix_expression.hpp"
#include "lexer/token/token.hpp"

namespace cmsl
{
    namespace exec
    {
        namespace infix
        {
            class fundamental_value_expression : public infix_expression
            {
            public:
                explicit fundamental_value_expression(lexer::token::token token);

                lexer::token::token get_token() const;
                
            protected:
                const lexer::token::token m_token;
            };

            class bool_value_expression : public fundamental_value_expression
            {
            public:
                explicit bool_value_expression(lexer::token::token token);

                inst::instance* evaluate(infix_evaluation_context& ctx) const override;
                void visit(infix_expression_visitor& visitor) const override;
            };

            class int_value_expression : public fundamental_value_expression
            {
            public:
                explicit int_value_expression(lexer::token::token token);

                inst::instance* evaluate(infix_evaluation_context& ctx) const override;
                void visit(infix_expression_visitor& visitor) const override;
            };

            class double_value_expression : public fundamental_value_expression
            {
            public:
                explicit double_value_expression(lexer::token::token token);

                inst::instance* evaluate(infix_evaluation_context& ctx) const override;
                void visit(infix_expression_visitor& visitor) const override;
            };

            class string_value_expression : public fundamental_value_expression
            {
            public:
                explicit string_value_expression(lexer::token::token token);

                inst::instance* evaluate(infix_evaluation_context& ctx) const override;
                void visit(infix_expression_visitor& visitor) const override;
            };

            std::unique_ptr<infix_expression> make_fundamental_expression(lexer::token::token token);
        }
    }
}