#pragma once

#include "ast/ast_node.hpp"
#include "lexer/token/token.hpp"

namespace cmsl
{
    namespace ast
    {
        class infix_node : public ast_node
        {
        private:
            using token_type_t = lexer::token::token_type;

        public:
            explicit infix_node(lexer::token::token_container_t tokens);

            int evaluate() const;

        private:
            lexer::token::token_container_t to_onp() const;

        private:
            lexer::token::token_container_t m_tokens;
        };
    }
}
