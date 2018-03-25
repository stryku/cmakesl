#pragma once

#include "lexer/token/token_type.hpp"

namespace cmsl
{
    namespace lexer
    {
        namespace token
        {
            class token
            {
            private:
                using token_t = cmsl::lexer::token::token_type;

            public:
                bool is_valid() const
                {
                    return m_type != token_t::undef;
                }

            private:
                token_t m_type{ token_t::undef };
            };
        }
    }
}
