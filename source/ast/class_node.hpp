#pragma once

#include "ast/ast_node.hpp"
#include "common/string.hpp"

#include "lexer/token/token.hpp"

namespace cmsl
{
    namespace ast
    {

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

            // Todo: return const vector ref
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
