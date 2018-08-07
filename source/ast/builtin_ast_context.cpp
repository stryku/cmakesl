#include "ast/builtin_ast_context.hpp"

#include "ast/type.hpp"
#include "ast/type_kind.hpp"

namespace cmsl
{
    namespace ast
    {
        builtin_ast_context::builtin_ast_context()
            : ast_context{ nullptr }
        {
            const auto builtin_types_kind = {
                type_kind::k_int,
                type_kind::k_real
            };

            for (const auto kind : builtin_types_kind)
            {
                add_type(std::make_unique<type>(type_kind::k_int));
            }
        }
    }
}

