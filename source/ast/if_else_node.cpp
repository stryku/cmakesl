#include "ast/if_else_node.hpp"
#include "ast/ast_node_visitor.hpp"

#include "ast/block_node.hpp"
#include "ast/conditional_node.hpp"

namespace cmsl::ast
{
        if_else_node::if_else_node(ifs_t ifs, std::unique_ptr<block_node> else_block)
            : ast_node{ ast_node_type::if_else }
            , m_ifs{ std::move(ifs) }
            , m_else{ std::move(else_block) }
        {}

        if_else_node::~if_else_node()
        {}

        const if_else_node::ifs_t &if_else_node::get_ifs() const
        {
            return m_ifs;
        }

        const block_node* if_else_node::get_else() const
        {
            return m_else.get();
        }

        void if_else_node::visit(ast_node_visitor &visitor) const
        {
            visitor.visit(*this);
        }
}

