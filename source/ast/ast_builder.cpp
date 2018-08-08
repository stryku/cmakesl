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

        std::unique_ptr<ast_node> ast_builder::build(errors::errors_observer& err_observer, const tokens_container_t& tokens)
        {
            m_parser = std::make_unique<parser>(err_observer, tokens);
            return m_parser->get_function();
        }
    }
}
