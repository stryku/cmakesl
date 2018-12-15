#pragma once

#include "ast/ast_node.hpp"
#include "ast/type_reference.hpp"

namespace cmsl
{
    namespace ast
    {
        class variable_declaration_node : public ast_node
        {
        public:
            explicit variable_declaration_node(type_reference ty, lexer::token::token name, std::unique_ptr<ast_node> initialization)
                : m_ty{ ty }
                , m_name{ name }
                , m_initialization{ std::move(initialization) }
            {}

        private:
            type_reference m_ty;
            lexer::token::token m_name;
            std::unique_ptr<ast_node> m_initialization;
        };
    }
}
