#pragma once

#include "ast/type_representation.hpp"

#include <optional>

namespace cmsl
{
    namespace ast
    {
        class type;
        struct parameter_declaration
        {
            const type* param_type;
            std::optional<lexer::token::token> name;
        };

        struct param_declaration
        {
            type_representation ty;
            lexer::token::token name;
        };
    }
}
