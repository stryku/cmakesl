#pragma once

#include "ast/ast_node.hpp"
#include "common/string.hpp"

#include "lexer/token.hpp"

#include <algorithm>

namespace cmsl::ast
{
    class class_node : public ast_node
    {
    public:
        using nodes_t = std::vector<std::unique_ptr<ast_node>>;

        class_node(token_t class_kw,
                    token_t name,
                    token_t open_brace,
                    nodes_t nodes,
                    token_t close_brace,
                    token_t semicolon)
            : m_class_kw{ class_kw }
            , m_name{ name }
            , m_open_brace{ open_brace }
            , m_nodes{ std::move(nodes) }
            , m_close_brace{ close_brace }
            , m_semicolon{ semicolon }
        {}

        token_t class_kw() const
        {
            return m_class_kw;
        }

        token_t name() const
        {
            return m_name;
        }

        token_t open_brace() const
        {
            return m_open_brace;
        }

        token_t close_brace() const
        {
            return m_close_brace;
        }

        token_t semicolon() const
        {
            return m_semicolon;
        }

        const nodes_t& nodes() const
        {
            return m_nodes;
        }

        void visit(ast_node_visitor &visitor) const override
        {
            visitor.visit(*this);
        }

        source_location begin_location() const override
        {
            return m_class_kw.src_range().begin;
        }

        source_location end_location() const override
        {
            return m_semicolon.src_range().end;
        }

    private:
        token_t m_class_kw;
        token_t m_name;
        token_t m_open_brace;
        nodes_t m_nodes;
        token_t m_close_brace;
        token_t m_semicolon;
    };
}
