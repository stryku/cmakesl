#pragma once

#include "lexer/token.hpp"

// Todo: do we need it?
namespace cmsl::sema
{
        class sema_type;
        struct member_info
        {
            lexer::token name;
            const sema_type& ty;
        };
}
