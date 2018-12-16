#include "ast/while_node.hpp"
#include "conditional_node.hpp"

namespace cmsl
{
    namespace ast
    {

        while_node::while_node(std::unique_ptr<conditional_node> node)
            : ast_node{ ast_node_type::while_ }
            , m_node{ std::move(node) }
        {}

        while_node::~while_node()
        {}

        const conditional_node &while_node::get_node() const
        {
            return *m_node;
        }
    }
}