#pragma once

#include "ast/ast_node.hpp"
#include "ast/parameter_declaration.hpp"
#include "ast/type.hpp"
#include "ast/block_node.hpp"

#include "lexer/token/token.hpp"

#include <memory>

namespace cmsl
{
    namespace ast
    {
        class ast_context;

        class function : public ast_node
        {
        public:
            function(ast_context& ctx,
                     type return_type,
                     cmsl::string_view name,
                     std::vector<parameter_declaration> params,
                     std::unique_ptr<block_node> body);

            cmsl::string_view get_name() const;
            const block_node& get_body() const;

            const ast_context& get_ast_context() const;

        private:
            ast_context& m_ast_context;
            type m_return_type;
            cmsl::string_view m_name;
            std::vector<parameter_declaration> m_params;
            std::unique_ptr<block_node> m_body;
        };
    }
}
