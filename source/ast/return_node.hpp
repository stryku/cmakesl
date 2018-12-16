#pragma once

#include "ast/ast_node.hpp"
#include "ast/infix_node.hpp"

#include <memory>

namespace cmsl
{
    namespace ast
    {
        class return_node : public ast_node
        {
        public:
            explicit return_node(std::unique_ptr<ast_node> expr);

            const ast_node& get_expression() const;

        private:
            std::unique_ptr<ast_node> m_expression;
        };
    }
}
