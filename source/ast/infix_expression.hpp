#pragma once

#include "ast/ast_node.hpp"
#include "lexer/token/token.hpp"

namespace cmsl
{
    namespace ast
    {
        class infix_expression : public ast_node
        {
        public:
            explicit infix_expression(lexer::token::token_container_t tokens);

            int evaluate();

        private:
            lexer::token::token_container_t m_tokens;
        };
    }
}
