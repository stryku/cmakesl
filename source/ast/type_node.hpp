#pragma once

#include "lexer/token/token.hpp"

namespace cmsl
{
    namespace ast
    {
        class type_node
        {
        public:
            explicit type_node(lexer::token::token token)
                : m_token{ token }
            {}

            lexer::token::token get_token() const
            {
                return m_token;
            }

        private:
            lexer::token::token m_token;
        };
    }
}
