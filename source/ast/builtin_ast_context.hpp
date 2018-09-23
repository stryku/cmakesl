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
        class builtin_ast_context : public ast_context_impl
        {
        public:
            builtin_ast_context();

        private:
            void add_types();
            void add_string_type();
            void add_version_type();

            void add_functions();
            void add_cmake_minimum_required();

            lexer::token::token fake_name_token() const;
        };
    }
}
