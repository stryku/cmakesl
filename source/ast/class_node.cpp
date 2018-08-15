#include "ast/class_node.hpp"

namespace cmsl
{
    namespace ast
    {
        class_node::class_node(string_view_map<const type*> members)
            : ast_node{ ast_node_type::class_type }
            , m_members{ std::move(members) }
        {}
    }
}
