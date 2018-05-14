#pragma once

#include "ast/ast_node.hpp"

#include <memory>

namespace cmsl
{
    namespace ast
    {
        class ast_builder
        {
        public:
            std::unique_ptr<ast_node> build();
        };
    }
}
