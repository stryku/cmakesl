#pragma once

#include "ast/type.hpp"
#include "lexer/token/token.hpp"

namespace cmsl
{
    namespace ast
    {
        struct parameter_declaration
        {
            const type* param_type;
            lexer::token::token name;
        };
    }
}
