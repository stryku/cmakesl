#include "ast/declaration_node.hpp"

namespace cmsl
{
    namespace ast
    {
        declaration_node::declaration_node(const type& t, cmsl::string_view name, std::unique_ptr<infix_node> expr)
            : ast_node{ ast_node_type::declaration }
            , m_type{ t }
            , m_name{ name }
            , m_expression{ std::move(expr) }
        {}

        const type& declaration_node::get_declared_type() const
        {
            return m_type;
        }

        cmsl::string_view declaration_node::get_name() const
        {
            return m_name;
        }

        const infix_node* declaration_node::get_expression() const
        {
            return m_expression.get();
        }
    }
}
