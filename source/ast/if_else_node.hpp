#pragma once

#include "ast/ast_node.hpp"

#include <memory>
#include <vector>

namespace cmsl
{
    namespace ast
    {
        class conditional_node;
        class block_node;

        class if_else_node : public ast_node
        {
        private:
            using ifs_t = std::vector<std::unique_ptr<conditional_node>>;

        public:
            explicit if_else_node();
            ~if_else_node();

            const ifs_t& get_ifs() const;
            const block_node& get_else() const;

        private:
            ifs_t m_ifs;
            std::unique_ptr<block_node> m_else;
        };
    }
}
