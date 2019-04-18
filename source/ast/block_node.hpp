#pragma once

#include "ast/ast_node.hpp"

#include <memory>
#include <vector>

namespace cmsl::ast
{
    class block_node : public ast_node
    {
    public:
        using expressions_t = std::vector<std::unique_ptr<ast_node>>;

        block_node(token_t open_brace, expressions_t expressions, token_t close_brace);

        token_t open_brace() const;
        token_t close_brace() const;

        // Todo: rename to get_nodes
        std::vector<const ast_node*> get_expressions() const;

        void visit(ast_node_visitor& visitor) const override;
        source_location begin_location() const override;
        source_location end_location() const override;

    private:
        token_t m_open_brace;
        expressions_t m_expressions;
        token_t m_close_brace;
    };
}
