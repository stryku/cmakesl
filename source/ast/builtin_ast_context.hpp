#pragma once

#include "ast/ast_context.hpp"

namespace cmsl
{
    namespace lexer
    {
        namespace token
        {
            class token;
        }
    }

    namespace ast
    {
        class builtin_ast_context : public ast_context
        {
        public:
            builtin_ast_context();

        private:
            void add_string_type();
            void add_version_type();

            lexer::token::token fake_name_token() const;
        };
    }
}
