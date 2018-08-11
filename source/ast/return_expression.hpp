#pragma once

#include "ast/ast_node.hpp"
#include "ast/infix_expression.hpp"

#include <memory>

namespace cmsl
{
    namespace exec
    {
        class executor;
    }

    namespace ast
    {
        class return_expression : public ast_node
        {
        public:
            explicit return_expression(std::unique_ptr<infix_expression> expr);

            void execute(exec::executor& executor);

        private:
            std::unique_ptr<infix_expression> m_expression;
        };
    }
}
