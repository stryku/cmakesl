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
            using expressions_t = std::vector<std::unique_ptr<ast_node>>;

            block_node(expressions_t expressions);

            // Todo: rename to get_nodes
            std::vector<const ast_node*> get_expressions() const;

            void visit(ast_node_visitor& visitor) const override;

        private:
            expressions_t m_expressions;
        };
    }
}
