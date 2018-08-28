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
                std::make_pair("int", type_kind::k_int),
                std::make_pair("real", type_kind::k_real)
            };

            for (const auto pair : builtin_types_kind)
            {
                add_type(std::make_unique<type>(pair.first, pair.second, nullptr));
            }
        }
    }
}

