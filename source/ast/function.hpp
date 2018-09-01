#pragma once

#include <common/string.hpp>
#include "ast/ast_node.hpp"

namespace cmsl
{
    namespace ast
    {
        class function : public ast_node
        {
        public:
            explicit function()
                : ast_node{ ast_node_type::function }
            {}

            virtual ~function() {}

            virtual cmsl::string_view get_name() const = 0;
        };
    }
}
