#include "ast/return_node.hpp"
#include "ast/ast_node_visitor.hpp"

namespace cmsl
{
    namespace ast
    {
        return_node::return_node(std::unique_ptr<ast_node> expr)
            : ast_node(ast_node_type::ret)
            , m_expression{ std::move(expr) }
        {}

        const ast_node& return_node::get_expression() const
        {
            return *m_expression;
        }

        void return_node::visit(ast_node_visitor &visitor) const
        {
            visitor.visit(*this);
        }
    }
}
