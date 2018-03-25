#pragma once

#include <string>

namespace cmsl
{
    namespace lexer
    {
        namespace token
        {
            class token;
        }

        class lexer
        {
        public:
            token::token get_token(const std::string& source) const;
        };
    }
}
