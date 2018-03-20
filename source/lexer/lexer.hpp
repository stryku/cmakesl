#pragma once

#include "lexer/token.hpp"

#include <string>

namespace cmsl
{
    namespace lexer
    {
        class lexer
        {
        public:
            token get_token(const std::string& source) const
            {
                return token{};
            }
        };
    }
}
