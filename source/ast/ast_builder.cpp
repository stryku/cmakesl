#include "ast/ast_builder.hpp"
#include "ast/builtin_ast_context.hpp"
//#include "ast/expr/block_expression.hpp"
//#include "ast/expr/constant_expression.hpp"
//#include "ast/expr/infix_expression.hpp"
#include "lexer/token/token.hpp"

#include "ast/binary_operator.hpp"
#include "ast/integral_constant.hpp"
#include "ast/infix_expression.hpp"
#include "ast/block_expression.hpp"
#include "ast/type.hpp"
#include "ast/function.hpp"

#include <stack>

namespace cmsl
{
    namespace ast
    {
        ast_builder::ast_builder()
        {
            auto builtin_ctx = std::make_unique<builtin_ast_context>();
            m_current_ast_ctx = builtin_ctx.get();
            m_contextes.emplace_back(std::move(builtin_ctx));
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

        bool ast_builder::is_infix_token(const token_t& token)
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

        std::unique_ptr<ast_node> ast_builder::build(const tokens_container_t& tokens)
        {
            m_current_token = tokens.cbegin();
            m_end = tokens.cend();
            return get_function();
        }

        void ast_builder::eat(token_t::token_type_t token_type)
        {
            if(m_current_token->get_type() != token_type)
            {
                int a; //todo
            }

            ++m_current_token;
        }

        std::unique_ptr<ast_node> ast_builder::infix()
        {
            tokens_container_t infix_tokens;

            while (is_infix_token(*m_current_token))
            {
                infix_tokens.push_back(*m_current_token);
                ++m_current_token;
            }

            eat(token_t::token_type_t::semicolon);

            return std::make_unique<infix_expression>(std::move(infix_tokens));
        }

        std::unique_ptr<ast_node> ast_builder::block_expr()
        {
            eat(token_t::token_type_t::open_brace);

            std::vector<std::unique_ptr<ast_node>> expressions;

            while (m_current_token->get_type() != token_t::token_type_t::close_brace)
            {
                expressions.emplace_back(infix());
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
            const auto t = *m_current_token;
            eat(token_t::token_type_t::identifier);
            return type{ t };
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
