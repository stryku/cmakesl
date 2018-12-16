#pragma once

#include "ast/ast_node.hpp"

#include <memory>

namespace cmsl
{
    namespace ast
    {
        class block_node;

        class conditional_node : public ast_node
        {
        public:
            explicit conditional_node(std::unique_ptr<ast_node> condition, std::unique_ptr<block_node> block);
            ~conditional_node();

            const ast_node& get_condition() const;
            const block_node& get_block() const;

            void visit(ast_node_visitor &visitor) const override;

        private:
            std::unique_ptr<ast_node> m_condition;
            std::unique_ptr<block_node> m_block;
        };
    }
}
