#pragma once

#include "lexer/token/token.hpp"

namespace cmsl
{
    namespace ast
    {
        class type
        {
        public:
            type(lexer::token::token token)
                : m_token{ token }
            {}

        private:
            lexer::token::token m_token;
        };
    }
}
