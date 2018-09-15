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

                inst::instance* evaluate(infix_evaluation_context& ctx) const override;

            private:
                const lexer::token::token m_token;
            };
        }
    }
}