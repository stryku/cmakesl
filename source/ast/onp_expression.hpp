#pragma once

#include "ast/ast_node.hpp"
#include "lexer/token/token.hpp"

namespace cmsl
{
    namespace ast
    {
        class onp_expression : public ast_node
        {
        public:
            onp_expression(lexer::token::token_container_t tokens)
                : m_tokens{ std::move(tokens) }
            {}

        private:
            lexer::token::token_container_t m_tokens;
        };
    }
}
