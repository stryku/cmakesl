#pragma once

#include "ast\expr\expression.hpp"
#include "lexer/token/token.hpp"

namespace cmsl
{
    namespace ast
    {
        // Expression that represents constant integrals, bools etc.
        class constant_expression : public expression
        {
        public:
            constant_expression(lexer::token::token tok);

            result_t evaluate() override;

        private:
            const lexer::token::token m_token;
        };
    }
}
