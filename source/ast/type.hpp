#pragma once

#include "lexer/token/token.hpp"

namespace cmsl
{
    namespace ast
    {
        enum class type_kind;

        class type
        {
        public:

            using token_type_t = lexer::token::token_type;

            explicit type(lexer::token::token token);
            explicit type(type_kind kind);

            bool is_builtin() const;

            cmsl::string_view get_name() const;

        private:
            cmsl::string_view m_name;
            type_kind m_kind;
        };
    }
}
