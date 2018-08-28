#include "ast/if_else_node.hpp"

#include "ast/block_node.hpp"
#include "ast/conditional_node.hpp"

namespace cmsl
{
    namespace ast
    {
        if_else_node::if_else_node()
            : ast_node{ ast_node_type::if_else }
        {}

        if_else_node::~if_else_node()
        {}

        const if_else_node::ifs_t &if_else_node::get_ifs() const
        {
            return m_ifs;
        }

        const block_node &if_else_node::get_else() const
        {
            return *m_else;
        }
    }
}

