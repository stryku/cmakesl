#include "ast/ast_builder.hpp"
#include "ast/expr/block_expression.hpp"
#include "ast/expr/constant_expression.hpp"
#include "lexer/token/token.hpp"

namespace cmsl
{
    namespace ast
    {
        std::unique_ptr<ast_node> ast_builder::build(const tokens_container_t& tokens)
        {
            m_current_token = tokens.cbegin();

            auto expr = build_block_expression();

        }

        bool ast_builder::is_constant_expression_token(const token_t& token)
        {
            const auto type = token.get_type();
            return type == lexer::token::token_type::integer ||
                type == lexer::token::token_type::real;
        }

        std::unique_ptr<expression> ast_builder::build_block_expression()
        {
            std::vector<std::unique_ptr<expression>> exprs;

            while (m_current_token != m_end &&
                   m_current_token->get_type() != lexer::token::token_type::close_brace)
            {
                if (is_constant_expression_token(*m_current_token))
                {
                    exprs.emplace_back(std::make_unique<constant_expression>());
                }

                ++m_current_token;
            }

            return std::make_unique<block_expression>(std::move(exprs));
        }
    }
}
