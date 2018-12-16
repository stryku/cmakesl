#pragma once

#include "ast/ast_node.hpp"

#include <memory>

namespace cmsl
{
    namespace ast
    {
        class conditional_node;

        class while_node : public ast_node
        {
        public:
            explicit while_node(std::unique_ptr<conditional_node> node);
            ~while_node();

            const conditional_node& get_node() const;

        private:
            std::unique_ptr<conditional_node> m_node;
        };
    }
}
