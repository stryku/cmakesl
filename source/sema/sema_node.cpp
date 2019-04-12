#include "sema/sema_node.hpp"

#include "ast/ast_node.hpp"
#include "sema_node.hpp"


namespace cmsl::sema
{
    sema::sema_node::sema_node(const ast::ast_node &ast_node)
         : m_ast_node{ ast_node }
    {}

    source_location sema_node::begin_location() const
    {
        return m_ast_node.begin_location();
    }

    source_location sema_node::end_location() const
    {
        return m_ast_node.end_location();
    }

    const ast::ast_node &sema_node::ast_node() const
    {
        return m_ast_node;
    }
}
