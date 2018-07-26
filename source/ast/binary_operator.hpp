#pragma once

#include "ast/ast_node.hpp"
#include "lexer/token/token.hpp"

namespace cmsl
{
    namespace ast
    {
        class binary_operator : public ast_node
        {
        public:
            binary_operator(ast_node* left, lexer::token::token op, ast_node* right)
                : m_left{ left }
                , m_operator{ op }
                , m_right{ right }
            {}

        private:
            ast_node* m_left;
            lexer::token::token m_operator;
            ast_node* m_right;
        };
    }
}
