#include "ast/builtin_ast_context.hpp"

#include "ast/type.hpp"
#include "ast/type_kind.hpp"
#include "ast/class_builder.hpp"
#include "ast/class_type.hpp"
#include "ast/fundamental_function.hpp"

namespace cmsl
{
    namespace ast
    {
        builtin_ast_context::builtin_ast_context()
            : ast_context{ nullptr }
        {
            const auto builtin_types_kind = {
                std::make_pair("int", type_kind::k_int),
                std::make_pair("double", type_kind::k_double),
                std::make_pair("bool", type_kind::k_bool)
            };

            for (const auto pair : builtin_types_kind)
            {
                add_type(std::make_unique<type>(pair.first, pair.second, nullptr));
            }

            add_string_type();
        }

        void builtin_ast_context::add_string_type()
        {
            class_builder builder{ *this, "string" };

            auto size_fun = std::make_unique<fundamental_function>(fundamental_function::fundamental_function_kind::size);
            builder.with_function(std::move(size_fun));

            auto string_class_node = builder.build();
            auto string_type = std::make_unique<class_type>(std::move(string_class_node), type_kind::k_string);

            add_type(std::move(string_type));
        }
    }
}

