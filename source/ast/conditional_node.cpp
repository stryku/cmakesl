#include "ast/conditional_node.hpp"

#include "ast/infix_node.hpp"
#include "ast/block_node.hpp"

namespace cmsl
{
    namespace ast
    {
        conditional_node::conditional_node(std::unique_ptr<infix_node> condition, std::unique_ptr<block_node> block)
            : ast_node{ ast_node_type::class_type }
            , m_condition{ std::move(condition) }
            , m_block{ std::move(m_block) }
        {}

        conditional_node::~conditional_node()
        {}

        const infix_node &conditional_node::get_condition() const
        {
            return *m_condition;
        }

        const block_node &conditional_node::get_block() const
        {
            return *m_block;
        }
    }
}
