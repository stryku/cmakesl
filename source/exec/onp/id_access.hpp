#pragma once

#include "common/string.hpp"

namespace cmsl
{
    namespace ast
    {
        class ast_context;
    }

    namespace exec
    {
        namespace inst
        {
            class instance;
        }

        class execution_context;

        namespace onp
        {
            struct id_access
            {
                inst::instance* parent_instance;
                cmsl::string_view name;

                bool is_function(const ast::ast_context& ctx) const;
                inst::instance* get_instance(execution_context& ctx);
            };
        }
    }
}
