#pragma once

#include "ast\expr\expression.hpp"
#include "lexer/token/token.hpp"

namespace cmsl
{
    namespace ast
    {
        class binary_operator_expression : public expression
        {
        public:
            binary_operator_expression(expression& left, expression& right, lexer::token::token tok);

            result_t evaluate() override;

        private:
            expression& m_left;
            expression& m_right;
            const lexer::token::token m_token;
        };
    }
}
