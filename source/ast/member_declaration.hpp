#pragma once

#include "ast/type.hpp"
#include "lexer/token/token.hpp"
#include "infix_node.hpp"

#include <memory>

namespace cmsl
{
    namespace ast
    {
        struct member_declaration
        {
            member_declaration(const type* t, lexer::token::token name, std::unique_ptr<infix_node> init_expression = nullptr)
                : type{ t }
                , name{ name }
                , init_expr{ std::move(init_expression) }
            {}

            const type* type;
            lexer::token::token name;
            std::unique_ptr<infix_node> init_expr;
        };
    }
}
