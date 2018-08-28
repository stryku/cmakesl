#pragma once

#include "ast/ast_node.hpp"

#include <memory>

namespace cmsl
{
    namespace ast
    {
        class infix_node;
        class block_node;

        class conditional_node : public ast_node
        {
        public:
            explicit conditional_node(std::unique_ptr<infix_node> condition, std::unique_ptr<block_node> block);
            ~conditional_node();

            const infix_node& get_condition() const;
            const block_node& get_block() const;

        private:
            std::unique_ptr<infix_node> m_condition;
            std::unique_ptr<block_node> m_block;
        };
    }
}
