#pragma once

#include "ast/ast_node.hpp"
#include "lexer/token/token.hpp"

namespace cmsl
{
    namespace ast
    {
        class integral_constant : public ast_node
        {
        public:
            integral_constant(lexer::token::token token)
                : m_token{ token }
            {}

        private:
            lexer::token::token m_token;
        };
    }
}
