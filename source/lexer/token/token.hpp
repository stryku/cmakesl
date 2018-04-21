#pragma once

#include "common/source_location.hpp"
#include "common/string.hpp"
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

                // Creates token with invalid begin and end locations
                explicit token(token_type_t type);
                explicit token(token_type_t type, source_range src_range);

                bool is_valid() const;
                token_type_t get_type() const;

                static token undef();

            private:
                token_type_t m_type{ token_type_t::undef };
                source_range m_source_range;
            };
        }
    }
}
