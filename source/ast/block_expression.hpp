#pragma once

#include "ast\ast_node.hpp"

#include <memory>

namespace cmsl
{
    namespace ast
    {
        class block_expression : public ast_node
        {
        public:
            block_expression(std::vector<std::unique_ptr<ast_node>> expressions)
                : m_expressions{ std::move(expressions) }
            {}

        private:
            std::vector<std::unique_ptr<ast_node>> m_expressions;
        };
    }
}
