#pragma once

#include "lexer/token/token.hpp"

#include "sema/sema_node.hpp"
#include "sema/sema_node_visitor.hpp"

namespace cmsl
{
    namespace ast
    {
        class type;
    }

    namespace sema
    {
        class variable_declaration_node : public sema_node
        {
        public:
            explicit variable_declaration_node(const ast::type& type, const lexer::token::token name, std::unique_ptr<sema_node> initialization)
                : m_type{ type }
                , m_name{ name }
                , m_initialization{ std::move(initialization) }
            {}

            void visit(sema_node_visitor& visitor) override
            {
                visitor.visit(*this);
            }

            const ast::type& type() const
            {
                return m_type;
            }

            lexer::token::token name() const
            {
                return m_name;
            }

            const sema_node* initialization() const
            {
                return m_initialization.get();
            }

        private:
            const ast::type& m_type;
            const lexer::token::token m_name;
            std::unique_ptr<sema_node> m_initialization;
        };
    }
}

