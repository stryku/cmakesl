#pragma once

#include "ast/ast_node.hpp"
#include "ast/infix_node.hpp"

#include <memory>

namespace cmsl
{
    namespace exec
    {
        class executor;
    }

    namespace ast
    {
        class return_node : public ast_node
        {
        public:
            explicit return_node(std::unique_ptr<infix_node> expr);

        private:
            std::unique_ptr<infix_node> m_expression;
        };
    }
}
