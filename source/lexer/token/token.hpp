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
            public:
                using token_type_t = cmsl::lexer::token::token_type;

                explicit token(token_type_t type = token_type_t::undef)
                    : m_type{ type }
                {}

                bool is_valid() const;
                token_type_t get_type() const;

            private:
                token_type_t m_type{ token_type_t::undef };
            };
        }
    }
}
