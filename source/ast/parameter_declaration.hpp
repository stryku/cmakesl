#pragma once

#include "lexer/token/token.hpp"

namespace cmsl
{
    namespace ast
    {
        class type;
        struct parameter_declaration
        {
            const type* param_type;
            lexer::token::token name; // todo introduce type_only_parameter_declaration
        };
    }
}
