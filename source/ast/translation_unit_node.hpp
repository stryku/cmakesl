#pragma once

#include "ast/ast_node.hpp"

#include <memory>
#include <vector>

namespace cmsl
{
    namespace ast
    {
        class translation_unit_node : public ast_node
        {
        public:
            explicit translation_unit_node(std::vector<std::unique_ptr<ast_node>> nodes)
                : m_nodes{ std::move(nodes)}
            {}

            std::vector<const ast_node*> get_nodes() const
            {
                std::vector<const ast_node*> result;

                std::transform(std::begin(m_nodes), std::end(m_nodes),
                               std::back_inserter(result),
                               [](const auto& node)
                               {
                                   return node.get();
                               });

                return result;
            }

        private:
            std::vector<std::unique_ptr<ast_node>> m_nodes;
        };
    }
}