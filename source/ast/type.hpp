#pragma once

#include "lexer/token/token.hpp"

namespace cmsl
{
    namespace ast
    {
        class type
        {
        public:
            using token_type_t = lexer::token::token_type;

            type(lexer::token::token token);

            bool is_builtin() const;

            cmsl::string_view get_name() const;

        private:
            lexer::token::token m_token;
        };
    }
}
