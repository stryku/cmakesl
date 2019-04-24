#pragma once

#include "ast/ast_node.hpp"
#include "ast/ast_node_visitor.hpp" // Todo: to cpp

#include <memory>
#include <vector>

#include <algorithm>

namespace cmsl::ast
{
        class translation_unit_node : public ast_node
        {
        public:
            using nodes_t = std::vector<std::unique_ptr<ast_node>>;

            explicit translation_unit_node(nodes_t nodes)
                : m_nodes{ std::move(nodes)}
            {}

            const nodes_t& nodes() const
            {
                return m_nodes;
            }

            void visit(ast_node_visitor& visitor) const override
            {
                visitor.visit(*this);
            }

            source_location begin_location() const override
            {
                return (!m_nodes.empty()) ? m_nodes.front()->begin_location() : source_location{};
            }

            source_location end_location() const override
            {
                return (!m_nodes.empty()) ? m_nodes.front()->end_location() : source_location{};
            }

        private:
            nodes_t m_nodes;
        };
}