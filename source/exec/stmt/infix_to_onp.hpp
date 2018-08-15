#pragma once

#include "lexer/token/token.hpp"

#include <stack>
#include <vector>

namespace cmsl
{
    namespace ast
    {
        class ast_context;
    }

    namespace exec
    {
        namespace stmt
        {
            class infix_to_onp
            {
            private:
                using tokens_container_t = std::vector<lexer::token::token>;
                using token_type_t = lexer::token::token_type;

            public:
                infix_to_onp(const tokens_container_t& infix_tokens, const ast::ast_context& ctx);

                tokens_container_t convert();

            private:
                void convert_function_call();
                void handle_token(const lexer::token::token& t);
                bool eat(token_type_t token_type);

            private:
                tokens_container_t::const_iterator m_token;
                tokens_container_t::const_iterator m_end;
                const ast::ast_context& m_ast_ctx;
                tokens_container_t m_out;
                std::stack<lexer::token::token> m_stack;
            };
        }
    }
}
