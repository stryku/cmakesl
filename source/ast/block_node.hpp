#pragma once

#include "ast/ast_node.hpp"

#include <memory>
#include <vector>

namespace cmsl
{
    namespace ast
    {
        class block_node : public ast_node
        {
        public:
            block_node(std::vector<std::unique_ptr<ast_node>> expressions);

            std::vector<ast_node*> get_expressions() const;

        private:
            std::vector<std::unique_ptr<ast_node>> m_expressions;
        };
    }
}
