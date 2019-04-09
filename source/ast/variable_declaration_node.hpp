#pragma once

#include "ast/ast_node.hpp"
#include "ast/type_representation.hpp"
#include "ast/ast_node_visitor.hpp" // todo: to cpp

namespace cmsl::ast
{
        class variable_declaration_node : public ast_node
        {
        public:
            struct initialization_values
            {
                token_t equal;
                std::unique_ptr<ast_node> initialization;
            };

            explicit variable_declaration_node(type_representation ty,
                                               token_t name,
                                               std::optional<initialization_values> initialization,
                                               token_t semicolon)
                : m_ty{ std::move(ty) }
                , m_name{ name }
                , m_initialization{ std::move(initialization) }
                , m_semicolon{ semicolon }
            {}

            //todo: remove get_

            const type_representation& get_type_representation() const
            {
                return m_ty;
            }

            token_t get_name() const
            {
                return m_name;
            }

            token_t semicolon() const
            {
                return m_semicolon;
            }

            std::optional<token_t> equal() const
            {
                if(m_initialization)
                {
                    return m_initialization->equal;
                }

                return std::nullopt;
            }

            const ast_node* get_initialization() const
            {
                return m_initialization ? m_initialization->initialization.get() : nullptr;
            }

            void visit(ast_node_visitor &visitor) const override
            {
                visitor.visit(*this);
            }

            source_location begin_location() const override
            {
                return m_ty.tokens().front().src_range().begin;
            }

            source_location end_location() const override
            {
                return m_semicolon.src_range().end;
            }

        private:
            type_representation m_ty;
            lexer::token::token m_name;
            std::optional<initialization_values> m_initialization;
            token_t m_semicolon;
        };
}
