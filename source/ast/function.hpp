#pragma once

#include "ast/ast_node.hpp"
#include "ast/parameter_declaration.hpp"
#include "ast/type.hpp"

#include "lexer/token/token.hpp"

#include <memory>

namespace cmsl
{
    namespace ast
    {
        class function : public ast_node
        {
        public:
            function(type return_type, cmsl::string_view name, std::vector<parameter_declaration> params, std::unique_ptr<ast_node> body);

            cmsl::string_view get_name() const;

        private:
            type m_return_type;
            cmsl::string_view m_name;
            std::vector<parameter_declaration> m_params;
            std::unique_ptr<ast_node> m_body;
        };
    }
}
