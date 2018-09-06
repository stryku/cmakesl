#pragma once

#include "common/string.hpp"
#include "ast/ast_node.hpp"
#include "ast/parameter_declaration.hpp"

namespace cmsl
{
    namespace ast
    {
        class function : public ast_node
        {
        public:
            using params_declaration_t = std::vector<parameter_declaration>;

            explicit function()
                : ast_node{ ast_node_type::function }
            {}

            virtual ~function() = default;

            virtual cmsl::string_view get_name() const = 0;
            virtual const params_declaration_t& get_params_declarations() const = 0;
        };
    }
}
