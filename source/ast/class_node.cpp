#include "ast/class_node.hpp"

namespace cmsl
{
    namespace ast
    {
        class_node::class_node(std::unique_ptr<ast_context> ast_ctx, cmsl::string_view name, std::vector<member_declaration> members)
            : ast_node{ ast_node_type::class_type }
            , m_ast_ctx{ std::move(ast_ctx) }
            , m_name{ name }
            , m_members{ std::move(members) }
        {}

        cmsl::string_view class_node::get_name() const
        {
            return m_name;
        }

        const std::vector<member_declaration>& class_node::get_members() const
        {
            return m_members;
        }

        const ast_context& class_node::get_ast_ctx() const
        {
            return *m_ast_ctx;
        }
    }
}
