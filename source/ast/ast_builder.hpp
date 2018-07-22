#pragma once

#include "ast/ast_node.hpp"

#include <memory>
#include <vector>

namespace cmsl
{
    namespace lexer
    {
        namespace token
        {
            class token;
        }
    }

    namespace ast
    {
        class expression;

        class ast_builder
        {
        private:
            using token_t = lexer::token::token;
            using tokens_container_t = std::vector<token_t>;
            using tokens_it_t = tokens_container_t::const_iterator;

        public:
            std::unique_ptr<expression> build(const tokens_container_t& tokens);

        private:
            bool is_constant_expression_token(const token_t& token);
            bool is_binary_operator_token(const token_t& token);

            std::unique_ptr<expression> basic_expression();
            std::unique_ptr<expression> expr();

            tokens_container_t inf_to_onp();

            std::unique_ptr<expression> build_block_expression();

        private:
            tokens_it_t m_current_token;
            tokens_it_t m_end;
        };
    }
}
