#include "ast/ast_context.hpp"

namespace cmsl
{
    namespace ast
    {
        ast_context::ast_context(ast_context* parent)
            : m_parent{ parent }
        {}
    }
}
