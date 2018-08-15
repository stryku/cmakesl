#include "ast/class_node.hpp"

namespace cmsl
{
    namespace ast
    {
        class_node::class_node(cmsl::string_view name, std::vector<member_declaration> members)
            : ast_node{ ast_node_type::class_type }
            , m_name{ name }
            , m_members{ std::move(members) }
        {}

        cmsl::string_view class_node::get_name() const
        {
            return m_name;
        }
    }
}
