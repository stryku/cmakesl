#pragma once

#include "lexer/token/token.hpp"

namespace cmsl
{
    namespace ast
    {
        struct type_reference
        {
            lexer::token::token begin;
            lexer::token::token end;

            cmsl::string_view to_string() const
            {
                const auto b = begin.str().data();
                return cmsl::string_view{ b, end.str().cend() - b };
            }
        };
    }
}
