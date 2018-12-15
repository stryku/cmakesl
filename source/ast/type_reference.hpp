#pragma once

namespace cmsl
{
    namespace ast
    {
        struct type_reference
        {
            lexer::token::token begin;
            lexer::token::token end;
        };
    }
}
