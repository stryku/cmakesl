#include "ast/if_else_node.hpp"
#include "ast/ast_node_visitor.hpp"

#include "ast/block_node.hpp"
#include "ast/conditional_node.hpp"
#include "if_else_node.hpp"


namespace cmsl::ast
{
        if_else_node::if_else_node(ifs_t ifs, std::optional<last_else_value> else_)
            : m_ifs{ std::move(ifs) }
            , m_else{ std::move(else_) }
        {}

        if_else_node::~if_else_node()
        {}

        const if_else_node::ifs_t &if_else_node::ifs() const
        {
            return m_ifs;
        }

        const block_node* if_else_node::else_body() const
        {
            return m_else ? m_else->body.get() : nullptr;
        }

        void if_else_node::visit(ast_node_visitor &visitor) const
        {
            visitor.visit(*this);
        }

    std::optional<ast_node::token_t> if_else_node::else_kw() const
    {
            if(m_else)
            {
                return m_else->else_kw;
            }

            return std::nullopt;
    }

    source_location if_else_node::begin_location() const
    {
        return m_ifs.front().if_kw.src_range().begin;
    }

    source_location if_else_node::end_location() const
    {
        return m_else ? m_else->body->end_location() : m_ifs.back().conditional->end_location();
    }
}

