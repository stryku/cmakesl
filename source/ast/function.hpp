#pragma once

#include "common/string.hpp"
#include "ast/ast_node.hpp"
#include "ast/parameter_declaration.hpp"

namespace cmsl::ast
{
        class function : public ast_node
        {
        public:
            using params_declaration_t = std::vector<parameter_declaration>;
            virtual ~function() = default;

            virtual cmsl::string_view get_name() const = 0;
            virtual const params_declaration_t& get_params_declarations() const = 0; // Todo: rename to get_param_declarations
            virtual const type& get_type() const = 0;

            // Todo: fixme
            void visit(ast_node_visitor& visitor) const override {};
        };
}
