#pragma once

#include "ast/ast_node.hpp"
#include "lexer/token/token.hpp"

#include <memory>

namespace cmsl
{
    namespace ast
    {
        class binary_operator : public ast_node
        {
        public:
            binary_operator(std::unique_ptr<ast_node> left, lexer::token::token op, std::unique_ptr<ast_node> right)
                : m_left{ std::move(left) }
                , m_operator{ op }
                , m_right{ std::move(right) }
            {}

        private:
            std::unique_ptr<ast_node> m_left;
            lexer::token::token m_operator;
            std::unique_ptr<ast_node> m_right;
        };
    }
}
