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

            // Todo: fixme
            void visit(ast_node_visitor& visitor) const override {};
        private:
            std::unique_ptr<ast_context> m_ast_ctx;
            cmsl::string_view m_name;
            std::vector<member_declaration> m_members;
        };

        // Todo: change name
        class class_node2 : public ast_node
        {
        public:
            using nodes_t = std::vector<std::unique_ptr<ast_node>>;

            class_node2(lexer::token::token name, nodes_t nodes)
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

            void visit(ast_node_visitor &visitor) const override
            {
                visitor.visit(*this);
            }

        private:
            lexer::token::token m_name;
            nodes_t m_nodes;
        };
    }
}
