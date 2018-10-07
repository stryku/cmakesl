#pragma once

#include "lexer/token/token.hpp"

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
    }
}
