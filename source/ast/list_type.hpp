#pragma once

#include "ast/type.hpp"
#include "type_kind.hpp"

namespace cmsl
{
    namespace ast
    {
        class list_type : public type
        {
        public:
            explicit list_type(cmsl::string_view name, const ast_context* ast_ctx, const type& value_type)
                : type{ name, type_kind::k_list, ast_ctx }
                , m_value_type{ value_type }
            {}

            const type& m_value_type;
        };
    }
}
