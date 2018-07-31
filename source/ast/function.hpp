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
            function(type t, lexer::token::token name, std::vector<parameter_declaration> params, std::unique_ptr<ast_node> body)
                : m_type{ t }
                , m_name{ name }
                , m_params{ std::move(params) }
                , m_body{ std::move(body) }
            {}

        private:
            type m_type;
            lexer::token::token m_name;
            std::vector<parameter_declaration> m_params;
            std::unique_ptr<ast_node> m_body;
        };
    }
}
