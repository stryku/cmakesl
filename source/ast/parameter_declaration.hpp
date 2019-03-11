#pragma once

#include "ast/type_name_reference.hpp"

#include <boost/optional.hpp>

namespace cmsl
{
    namespace ast
    {
        class type;
        struct parameter_declaration
        {
            const type* param_type;
            boost::optional<lexer::token::token> name;
        };

        struct param_declaration
        {
            type_name_reference ty;
            lexer::token::token name;
        };
    }
}
