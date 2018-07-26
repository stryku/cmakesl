#include "ast/ast_builder.hpp"
#include "ast/expr/block_expression.hpp"
#include "ast/expr/constant_expression.hpp"
#include "ast/expr/onp_expression.hpp"
#include "lexer/token/token.hpp"

#include "ast/binary_operator.hpp"
#include "ast/integral_constant.hpp"
#include "ast/onp_expression.hpp"
#include "ast/block_expression.hpp"
#include "ast/type.hpp"
#include "ast/function.hpp"

#include <stack>

namespace cmsl
{
    namespace ast
    {
       /* std::unique_ptr<expression> ast_builder::build(const tokens_container_t& tokens)
        {
            m_current_token = tokens.cbegin();
            m_end = tokens.cend();
            return expr();
        }
*/
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

        bool ast_builder::is_onp_token(const token_t& token)
        {
            const auto allowed_tokens = {
                token_t::token_type_t::integer,
                token_t::token_type_t::real,
                token_t::token_type_t::plus,
                token_t::token_type_t::minus,
                token_t::token_type_t::open_paren,
                token_t::token_type_t::close_paren
            };

            return std::any_of(std::cbegin(allowed_tokens), std::cend(allowed_tokens),
                               [&token](const auto token_type)
                               {
                                   return token.get_type() == token_type;
                               });
        }


        //std::unique_ptr<expression> ast_builder::basic_expression()
        //{
        //    if (is_constant_expression_token(*m_current_token))
        //    {
        //        return std::make_unique<constant_expression>(*m_current_token);
        //    }
        //    else if (m_current_token->get_type() == lexer::token::token_type::open_paren)
        //    {
        //        ++m_current_token;
        //        //auto e = expr();
        //        //todo: check if current token is right param
        //        ++m_current_token;
        //        //return std::move(e);
        //    }
        //    else
        //    {
        //        //todo: raise error
        //    }
        //}

        /*std::unique_ptr<expression> ast_builder::expr()
        {
            const auto onp_tokens = inf_to_onp();
            return std::make_unique<onp_expression>(std::move(onp_tokens));
        }*/

        //ast_builder::tokens_container_t ast_builder::inf_to_onp()
        //{
        //    tokens_container_t out;
        //    std::stack<token_t> stack;

        //    while (m_current_token != m_end)
        //    {
        //        if (is_constant_expression_token(*m_current_token))
        //        {
        //            out.push_back(*m_current_token);
        //        }
        //        else if (is_binary_operator_token(*m_current_token))
        //        {
        //            while (!stack.empty() && is_binary_operator_token(stack.top()))
        //            {
        //                out.push_back(stack.top());
        //                stack.pop();
        //            }

        //            stack.push(*m_current_token);
        //        }
        //        else if (m_current_token->get_type() == lexer::token::token_type::open_paren)
        //        {
        //            stack.push(*m_current_token);
        //        }
        //        else if (m_current_token->get_type() == lexer::token::token_type::close_paren)
        //        {
        //            if (stack.empty())
        //            {
        //                //todo handle error, wrong parens
        //            }

        //            while (stack.top().get_type() != lexer::token::token_type::open_paren)
        //            {
        //                out.push_back(stack.top());
        //                stack.pop();

        //                if (stack.empty())
        //                {
        //                    //todo handle error, wrong parens
        //                }
        //            }

        //            stack.pop();
        //        }

        //        ++m_current_token;
        //    }

        //    while (!stack.empty() && is_binary_operator_token(stack.top()))
        //    {
        //        out.push_back(stack.top());
        //        stack.pop();
        //    }

        //    return out;
        //}

        //std::unique_ptr<expression> ast_builder::build_block_expression()
        //{
        //    std::vector<std::unique_ptr<expression>> exprs;

        //    while (m_current_token != m_end &&
        //           m_current_token->get_type() != lexer::token::token_type::close_brace)
        //    {
        //        if (is_constant_expression_token(*m_current_token))
        //        {
        //            exprs.emplace_back(std::make_unique<constant_expression>(*m_current_token));
        //        }
        //        else if (is_binary_operator_token(*m_current_token))
        //        {

        //        }

        //        ++m_current_token;
        //    }

        //    return std::make_unique<block_expression>(std::move(exprs));
        //}

        std::unique_ptr<ast_node> ast_builder::build2(const tokens_container_t& tokens)
        {
            m_current_token = tokens.cbegin();
            m_end = tokens.cend();
            return get_function();
        }

        //std::unique_ptr<ast_node> ast_builder::expr2()
        //{
        //    while (m_current_token != m_end)
        //    {
        //        if (is_constant_expression_token(*m_current_token))
        //        {
        //            ++m_current_token;
        //            return std::make_unique<integral_constant>(*m_current_token);
        //        }
        //        else if (m_current_token->get_type() != lexer::token::token_type::open_paren)
        //        {
        //            ++m_current_token;//lparen
        //            return expr2();
        //            ++m_current_token; //rparen
        //        }
        //        else
        //        {
        //            auto lhs = expr2();
        //            auto bin_op_token = *m_current_token;
        //            auto rhs = expr2();
        //            return std::make_unique<binary_operator>(std::move(lhs), bin_op_token, std::move(rhs));
        //        }
        //    }
        //}

        void ast_builder::eat(token_t::token_type_t token_type)
        {
            if(m_current_token->get_type() != token_type)
            {
                int a; //todo
            }

            ++m_current_token;
        }

        std::unique_ptr<ast_node> ast_builder::onp()
        {
            tokens_container_t onp_tokens;

            while (is_onp_token(*m_current_token))
            {
                onp_tokens.push_back(*m_current_token);
                ++m_current_token;
            }

            eat(token_t::token_type_t::semicolon);

            return std::make_unique<onp_expression>(std::move(onp_tokens));
        }

        std::unique_ptr<ast_node> ast_builder::block_expr()
        {
            eat(token_t::token_type_t::open_brace);

            std::vector<std::unique_ptr<ast_node>> expressions;

            while (m_current_token->get_type() != token_t::token_type_t::close_brace)
            {
                expressions.emplace_back(onp());
            }

            eat(token_t::token_type_t::close_brace);

            return std::make_unique<block_expression>(std::move(expressions));
        }

        std::unique_ptr<ast_node> ast_builder::get_function()
        {
            const auto t = get_type();

            const auto name = *m_current_token;
            eat(token_t::token_type_t::identifier);

            eat(token_t::token_type_t::open_paren);

            auto params = parameters();

            eat(token_t::token_type_t::close_paren);

            auto block = block_expr();

            return std::make_unique<function>(t, name, std::move(params), std::move(block));
        }

        type ast_builder::get_type()
        {
            eat(token_t::token_type_t::identifier);
            return type{};
        }

        std::vector<parameter_declaration> ast_builder::parameters()
        {
            std::vector<parameter_declaration> params;

            while (m_current_token->get_type() != token_t::token_type_t::close_paren)
            {
                auto t = get_type();
                auto name = *m_current_token;
                eat(token_t::token_type_t::identifier);

                if (m_current_token->get_type() == token_t::token_type_t::semicolon);
                {
                    eat(token_t::token_type_t::semicolon);
                } // todo handle semicolon without param

                params.push_back({ t, name });
            }

            return params;
        }
    }
}
