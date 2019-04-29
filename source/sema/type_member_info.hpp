#pragma once

#include "lexer/token.hpp"

namespace cmsl::sema
{
        class sema_type;
        struct member_info
        {
            lexer::token name;
            const sema_type& ty;
        };
}
