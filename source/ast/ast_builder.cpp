#include "ast/ast_builder.hpp"
#include "ast/expr/block_expression.hpp"
#include "ast/expr/constant_expression.hpp"
#include "ast/expr/onp_expression.hpp"
#include "lexer/token/token.hpp"

#include <stack>

namespace cmsl
{
    namespace ast
    {
        std::unique_ptr<expression> ast_builder::build(const tokens_container_t& tokens)
        {
            m_current_token = tokens.cbegin();
            m_end = tokens.cend();
            return expr();
        }

        bool ast_builder::is_constant_expression_token(const token_t& token)
        {
            const auto type = token.get_type();
            return type == lexer::token::token_type::integer ||
                type == lexer::token::token_type::real;
        }

        bool ast_builder::is_binary_operator_token(const token_t& token)
        {
            const auto type = token.get_type();
            return type == lexer::token::token_type::plus ||
                type == lexer::token::token_type::minus;
        }

        std::unique_ptr<expression> ast_builder::basic_expression()
        {
            if (is_constant_expression_token(*m_current_token))
            {
                return std::make_unique<constant_expression>(*m_current_token);
            }
            else if (m_current_token->get_type() == lexer::token::token_type::open_paren)
            {
                ++m_current_token;
                auto e = expr();
                //todo: check if current token is right param
                ++m_current_token;
                return std::move(e);
            }
            else
            {
                //todo: raise error
            }
        }

        std::unique_ptr<expression> ast_builder::expr()
        {
            const auto onp_tokens = inf_to_onp();
            return std::make_unique<onp_expression>(std::move(onp_tokens));
        }

        ast_builder::tokens_container_t ast_builder::inf_to_onp()
        {
            tokens_container_t out;
            std::stack<token_t> stack;

            while (m_current_token != m_end)
            {
                if (is_constant_expression_token(*m_current_token))
                {
                    out.push_back(*m_current_token);
                }
                else if (is_binary_operator_token(*m_current_token))
                {
                    while (!stack.empty() && is_binary_operator_token(stack.top()))
                    {
                        out.push_back(stack.top());
                        stack.pop();
                    }

                    stack.push(*m_current_token);
                }
                else if (m_current_token->get_type() == lexer::token::token_type::open_paren)
                {
                    stack.push(*m_current_token);
                }
                else if (m_current_token->get_type() == lexer::token::token_type::close_paren)
                {
                    if (stack.empty())
                    {
                        //todo handle error, wrong parens
                    }

                    while (stack.top().get_type() != lexer::token::token_type::open_paren)
                    {
                        out.push_back(stack.top());
                        stack.pop();

                        if (stack.empty())
                        {
                            //todo handle error, wrong parens
                        }
                    }

                    stack.pop();
                }

                ++m_current_token;
            }

            while (!stack.empty() && is_binary_operator_token(stack.top()))
            {
                out.push_back(stack.top());
                stack.pop();
            }

            return out;
        }

        std::unique_ptr<expression> ast_builder::build_block_expression()
        {
            std::vector<std::unique_ptr<expression>> exprs;

            while (m_current_token != m_end &&
                   m_current_token->get_type() != lexer::token::token_type::close_brace)
            {
                if (is_constant_expression_token(*m_current_token))
                {
                    exprs.emplace_back(std::make_unique<constant_expression>(*m_current_token));
                }
                else if (is_binary_operator_token(*m_current_token))
                {

                }

                ++m_current_token;
            }

            return std::make_unique<block_expression>(std::move(exprs));
        }

    }
}
