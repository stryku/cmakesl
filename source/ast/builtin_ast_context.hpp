#pragma once

#include "ast/ast_context.hpp"

namespace cmsl
{
    namespace ast
    {
        class builtin_ast_context : public ast_context
        {
        public:
            builtin_ast_context();
        };
    }
}
