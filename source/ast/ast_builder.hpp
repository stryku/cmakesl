#pragma once

#include "ast/ast_node.hpp"
#include "ast/type.hpp"
#include "ast/ast_context.hpp"
#include "lexer/token/token.hpp"

#include <memory>
#include <vector>

namespace cmsl
{
    namespace ast
    {
        class expression;
        class type;
        struct parameter_declaration;

        class ast_builder
        {
        private:
            using token_t = lexer::token::token;
            using tokens_container_t = std::vector<token_t>;
            using tokens_it_t = tokens_container_t::const_iterator;

        public:
            explicit ast_builder();

            std::unique_ptr<ast_node> build(const tokens_container_t& tokens);

        private:
            bool is_constant_expression_token(const token_t& token);
            bool is_binary_operator_token(const token_t& token);
            bool is_infix_token(const token_t& token);

            void eat(lexer::token::token_type token_type);

            std::unique_ptr<ast_node> infix();
            std::unique_ptr<ast_node> block_expr();
            std::unique_ptr<ast_node> get_function();

            type get_type();

            std::vector<parameter_declaration> parameters();

        private:
            tokens_it_t m_current_token;
            tokens_it_t m_end;
            ast_context* m_current_ast_ctx;
            std::vector<std::unique_ptr<ast_context>> m_contextes;
        };
    }
}
