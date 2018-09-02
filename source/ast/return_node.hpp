#pragma once

#include "ast/ast_node.hpp"
#include "ast/infix_node.hpp"

#include <memory>

namespace cmsl
{
    namespace exec
    {
        class source_executor;
    }

    namespace ast
    {
        class return_node : public ast_node
        {
        public:
            explicit return_node(std::unique_ptr<infix_node> expr);

            infix_node& get_expression();

        private:
            std::unique_ptr<infix_node> m_expression;
        };
    }
}
