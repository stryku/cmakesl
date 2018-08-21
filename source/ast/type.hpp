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
            explicit type(cmsl::string_view name, type_kind kind);
            virtual ~type() {}

            bool is_builtin() const; // todo consider rename to is_fundamental

            cmsl::string_view get_name() const;

            bool has_function(cmsl::string_view name) const;

        private:
            cmsl::string_view m_name;
            type_kind m_kind;
        };
    }
}
