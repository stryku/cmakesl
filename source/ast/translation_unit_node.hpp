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

        private:
            std::vector<std::unique_ptr<ast_node>> m_nodes;
        };
    }
}