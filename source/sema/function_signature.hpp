#pragma once

#include "lexer/token/token.hpp"
#include <vector>

namespace cmsl
{
    namespace sema
    {
        class sema_type;

        struct parameter_declaration
        {
            const sema_type& ty;
            lexer::token::token name;
        };

        struct function_signature
        {
            lexer::token::token name;
            std::vector<parameter_declaration> params{};
        };
    }
}
