#include "ast/parser.hpp"
#include "ast/ast_context.hpp"
#include "ast/type.hpp"
#include "ast/infix_expression.hpp"
#include "ast/block_expression.hpp"
#include "ast/function.hpp"

#include "common/algorithm.hpp"

#include "errors/error.hpp"
#include "errors/errors_observer.hpp"


namespace cmsl
{
    namespace ast
    {
        parser::parser(errors::errors_observer& err_observer, const lexer::token::token_container_t& tokens)
            : m_err_observer{ err_observer }
            , m_token{ tokens.cbegin() }
            , m_end{ tokens.cend() }
        {}

        void parser::raise_error()
        {
            m_err_observer.nofify_error(errors::error{});
        }

        bool parser::eat(boost::optional<lexer::token::token_type> type)
        {
            if (is_at_end())
            {
                raise_error();
                return false;
            }

            if (type && !current_is(*type))
            {
                raise_error();
                return false;
            }

            ++m_token;
            return true;
        }

        bool parser::eat_type()
        {
            const auto token_type = m_token->get_type();

            if (is_builtin_type(token_type) ||
                token_type == token_type_t::identifier)
            {
                eat();
                return true;
            }
            else
            {
                raise_error();
                return false;
            }
        }

        bool parser::is_builtin_type(token_type_t token_type) const
        {
            const auto builtin_types = {
                token_type_t::t_int,
                token_type_t::t_real
            };

            return cmsl::contains(builtin_types, token_type);
        }

        type* parser::get_type(ast_context& ctx)
        {
            const auto t = *m_token;

            if (!eat_type())
            {
                return nullptr;
            }

            auto found_type = ctx.find_type(t.str());
            if (found_type == nullptr)
            {
                raise_error(); //todo raise type not found error
                return nullptr;
            }

            return found_type;
        }

        boost::optional<std::vector<parameter_declaration>> parser::get_parameters_declaration(ast_context& ctx)
        {
            std::vector<parameter_declaration> params;

            if (!eat(token_type_t::open_paren))
            {
                return boost::none;
            }

            while (true)
            {
                if (is_at_end())
                {
                    // Unexpected end of tokens
                    raise_error();
                    return boost::none;
                }

                if (current_is(token_type_t::close_paren))
                {
                    // End of parameters
                    break;
                }

                const auto param_decl = get_parameter_declaration(ctx);
                if (!param_decl)
                {
                    return boost::none;
                }

                params.push_back(std::move(*param_decl));

                if (!prepare_for_next_parameter_declaration())
                {
                    return boost::none;
                }
            }

            if(!eat(token_type_t::close_paren))
            {
                return boost::none;
            }

            return params;
        }

        bool parser::prepare_for_next_parameter_declaration()
        {
            if (!expect_not_at_end())
            {
                return false;
            }

            if (!current_is(token_type_t::semicolon))
            {
                return true;
            }

            // At this point we have semicolon, so we expect next parameter

            if (!eat(token_type_t::semicolon))
            {
                return false;
            }

            if (!expect_not_at_end())
            {
                return false;
            }

            if (m_token->get_type() == token_type_t::close_paren)
            {
                // Missed last parameter declaration
                raise_error();
                return false;
            }

            return true;
        }

        boost::optional<parameter_declaration> parser::get_parameter_declaration(ast_context& ctx)
        {
            auto t = get_type(ctx);

            if (!t)
            {
                return{};
            }

            auto name = *m_token;
            if (!eat(token_type_t::identifier))
            {
                return{};
            }
            
            return parameter_declaration{ *t, name };
        }

        bool parser::is_at_end() const
        {
            return m_token == m_end;
        }

        bool parser::expect_token(token_type_t type)
        {
            if (!expect_not_at_end())
            {
                return false;
            }

            if (m_token->get_type() != type)
            {
                raise_error();
                return false;
            }

            return true;
        }

        bool parser::expect_token_other_than(token_type_t type)
        {
            return expect_not_at_end() && m_token->get_type() != type;
        }

        bool parser::expect_not_at_end()
        {
            if (is_at_end())
            {
                raise_error();
                return false;
            }

            return true;
        }

        bool parser::current_is(token_type_t token_type) const
        {
            return m_token->get_type() == token_type;
        }

        std::unique_ptr<ast_node> parser::get_infix_expression()
        {
            token_container_t infix_tokens;

            while (current_is_infix_token())
            {
                infix_tokens.push_back(*m_token);
                eat();
            }

            if (!expect_token(token_type_t::semicolon))
            {
                return nullptr;
            }

            eat(token_type_t::semicolon);

            return std::make_unique<infix_expression>(std::move(infix_tokens));
        }

        bool parser::current_is_infix_token() const
        {
            if (is_at_end())
            {
                return false;
            }

            const auto allowed_tokens = {
                token_type_t::integer,
                token_type_t::real,
                token_type_t::plus,
                token_type_t::minus,
                token_type_t::open_paren,
                token_type_t::close_paren
            };

            return cmsl::contains(allowed_tokens, m_token->get_type());
        }

        std::unique_ptr<ast_node> parser::get_block_expression()
        {
            if (!eat(token_type_t::open_brace))
            {
                return nullptr;
            }

            std::vector<std::unique_ptr<ast_node>> expressions;

            while (!is_at_end() && !current_is(token_type_t::close_brace))
            {
                auto infix_expr = get_infix_expression();
                if (!infix_expr)
                {
                    return nullptr;
                }

                expressions.emplace_back(std::move(infix_expr));
            }

            if (!eat(token_type_t::close_brace))
            {
                return nullptr;
            }

            return std::make_unique<block_expression>(std::move(expressions));
        }

        std::unique_ptr<ast_node> parser::get_function(ast_context& ctx)
        {
            const auto type = get_type(ctx);
            if (!type)
            {
                return nullptr;
            }

            const auto name = get_identifier();
            if (!name)
            {
                return nullptr;
            }

            auto parameters = get_parameters_declaration(ctx);
            if (!parameters)
            {
                return nullptr;
            }

            auto block_expr = get_block_expression();

            if (!block_expr)
            {
                return nullptr;
            }

            return std::make_unique<function>(*type, *name, std::move(*parameters), std::move(block_expr));
        }

        boost::optional<lexer::token::token> parser::get_identifier()
        {
            if (!expect_not_at_end())
            {
                return boost::none;
            }

            if (!current_is(token_type_t::identifier))
            {
                raise_error();
                return boost::none;
            }

            const auto id = *m_token;
            eat();

            return id;
        }
    }
}
