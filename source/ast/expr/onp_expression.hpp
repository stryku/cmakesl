#pragma once

#include "ast\expr\expression.hpp"
#include "lexer/token/token.hpp"

namespace cmsl
{
    namespace ast
    {
        class onp_expression : public expression
        {
        public:
            onp_expression(lexer::token::token_container_t tokens);

            result_t evaluate() override;

        private:
            const lexer::token::token_container_t m_tokens;
        };
    }
}
