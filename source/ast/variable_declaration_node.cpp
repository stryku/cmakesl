#include "ast/variable_declaration_node.hpp"

#include "ast/ast_node_visitor.hpp"

namespace cmsl::ast
{
    variable_declaration_node::variable_declaration_node(type_representation ty,
                                                         token_t name,
                                                         std::optional<initialization_values> initialization,
                                                         token_t semicolon)
            : m_type{ std::move(ty) }
            , m_name{ name }
            , m_initialization{ std::move(initialization) }
            , m_semicolon{ semicolon }
    {}

    const type_representation& variable_declaration_node::type() const
    {
        return m_type;
    }

    variable_declaration_node::token_t variable_declaration_node::name() const
    {
        return m_name;
    }

    variable_declaration_node::token_t variable_declaration_node::semicolon() const
    {
        return m_semicolon;
    }

    std::optional<variable_declaration_node::token_t> variable_declaration_node::equal() const
    {
        if(m_initialization)
        {
            return m_initialization->equal;
        }

        return std::nullopt;
    }

    const ast_node* variable_declaration_node::initialization() const
    {
        return m_initialization ? m_initialization->initialization.get() : nullptr;
    }

    void variable_declaration_node::visit(ast_node_visitor &visitor) const
    {
        visitor.visit(*this);
    }

    source_location variable_declaration_node::begin_location() const
    {
        return m_type.tokens().front().src_range().begin;
    }

    source_location variable_declaration_node::end_location() const
    {
        return m_semicolon.src_range().end;
    }
}
