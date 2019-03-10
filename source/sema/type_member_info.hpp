#pragma once

#include "lexer/token/token.hpp"

namespace cmsl
{
    namespace sema
    {
        class sema_type;
        struct member_info
        {
            lexer::token::token name;
            const sema_type& ty;
        };
    }
}
