#pragma once

#include "ast/ast_node.hpp"
#include "ast/type_name_reference.hpp"
#include "ast/ast_node_visitor.hpp" // todo: to cpp

namespace cmsl
{
    namespace ast
    {
        class variable_declaration_node : public ast_node
        {
        public:
            explicit variable_declaration_node(type_name_reference ty, lexer::token::token name, std::unique_ptr<ast_node> initialization)
                : m_ty{ ty }
                , m_name{ name }
                , m_initialization{ std::move(initialization) }
            {}

            type_name_reference get_type_reference() const
            {
                return m_ty;
            }

            lexer::token::token get_name() const
            {
                return m_name;
            }

            const ast_node* get_initialization() const
            {
                return m_initialization.get();
            }

            void visit(ast_node_visitor &visitor) const override
            {
                visitor.visit(*this);
            }

        private:
            type_name_reference m_ty;
            lexer::token::token m_name;
            std::unique_ptr<ast_node> m_initialization;
        };
    }
}
