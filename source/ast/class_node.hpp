#pragma once

#include "ast/ast_context.hpp"
#include "ast/ast_node.hpp"
#include "ast/type.hpp"
#include "common/string.hpp"
#include "ast/member_declaration.hpp"

namespace cmsl
{
    namespace ast
    {
        class ast_context;

        class class_node : public ast_node
        {
        public:
            class_node(std::unique_ptr<ast_context> ast_ctx, cmsl::string_view name, std::vector<member_declaration> m_members);

            cmsl::string_view get_name() const;
            const std::vector<member_declaration>& get_members() const;
            const ast_context& get_ast_ctx() const;

        private:
            std::unique_ptr<ast_context> m_ast_ctx;
            cmsl::string_view m_name;
            std::vector<member_declaration> m_members;
        };

        class class_node2 : public ast_node
        {
        public:
            class_node2(lexer::token::token name, std::vector<std::unique_ptr<ast_node>> nodes)
                : m_name{ name }
                , m_nodes{ std::move(nodes) }
            {}

            lexer::token::token get_name() const
            {
                return m_name;
            }

            std::vector<const ast_node*> get_nodes() const
            {
                std::vector<const ast_node*> nodes;

                // Todo: lazy init and store
                std::transform(std::begin(m_nodes), std::end(m_nodes),
                              std::back_inserter(nodes),
                              [](const auto& unique_node)
                               {
                                   return unique_node.get();
                               });

                return nodes;
            }

        private:
            lexer::token::token m_name;
            std::vector<std::unique_ptr<ast_node>> m_nodes;
        };
    }
}
