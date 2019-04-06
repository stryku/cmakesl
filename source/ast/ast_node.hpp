#pragma once

#include "ast/ast_node_type.hpp"

namespace cmsl::ast
{
    class ast_node_visitor;

    class ast_node
    {
    public:
        ast_node()
        {}

        ast_node(ast_node_type type)
            : m_type{ type }
        {}

        virtual ~ast_node()
        {}

        ast_node_type get_type() const
        {
            return m_type;
        }

        virtual void visit(ast_node_visitor& visitor) const = 0;

    private:
        ast_node_type m_type;
    };
}
