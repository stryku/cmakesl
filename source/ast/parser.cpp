#include "ast/parser.hpp"
#include "ast/ast_context.hpp"
#include "ast/type.hpp"
#include "ast/infix_node.hpp"
#include "ast/block_node.hpp"
#include "ast/function_node.hpp"
#include "ast/return_node.hpp"
#include "ast/infix_node.hpp"
#include "ast/declaration_node.hpp"

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

        const type* parser::get_type(ast_context& ctx) 
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

            if (!current_is(token_type_t::comma))
            {
                return true;
            }

            // At this point we have comma, so we expect next parameter

            if (!eat(token_type_t::comma))
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
            
            return parameter_declaration{ t, name };
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

        std::unique_ptr<infix_node> parser::get_infix_node()
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

            return std::make_unique<infix_node>(std::move(infix_tokens));
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
                token_type_t::close_paren,
                token_type_t::identifier,
                token_type_t::comma
            };

            return cmsl::contains(allowed_tokens, m_token->get_type());
        }

        std::unique_ptr<block_node> parser::get_block_node(ast_context& ctx)
        {
            if (!eat(token_type_t::open_brace))
            {
                return nullptr;
            }

            std::vector<std::unique_ptr<ast_node>> expressions;

            while (!is_at_end() && !current_is(token_type_t::close_brace))
            {
                std::unique_ptr<ast_node> expr;

                if (current_is(token_type_t::kw_return))
                {
                    expr = get_return_node();
                }
                else if (declaration_starts())
                {
                    expr = get_declaration_node(ctx);
                }
                else
                {
                    expr = get_infix_node();
                }

                if (!expr)
                {
                    return nullptr;
                }
                expressions.emplace_back(std::move(expr));
            }

            if (!eat(token_type_t::close_brace))
            {
                return nullptr;
            }

            return std::make_unique<block_node>(std::move(expressions));
        }

        std::unique_ptr<function_node> parser::get_function(ast_context& ctx)
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

            auto block_expr = get_block_node(ctx);

            if (!block_expr)
            {
                return nullptr;
            }

            return std::make_unique<function_node>(ctx, *type, name->str(), std::move(*parameters), std::move(block_expr));
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

        std::unique_ptr<return_node> parser::get_return_node()
        {
            if (!eat(token_type_t::kw_return))
            {
                raise_error();
                return nullptr;
            }

            auto infix_expr = get_infix_node();
            if (!infix_expr)
            {
                return nullptr;
            }

            return std::make_unique<return_node>(std::move(infix_expr));
        }

        parser::token_type_t parser::peek(size_t n) const
        {
            if (std::distance(m_token, m_end) <= n)
            {
                return token_type_t::undef;
            }

            return std::next(m_token, n)->get_type();
        }


        bool parser::declaration_starts() const
        {
            // 2 because 
            // type name =
            return peek(2u) == token_type_t::equal;
        }

        std::unique_ptr<declaration_node> parser::get_declaration_node(ast_context& ctx)
        {
            auto type = get_type(ctx);
            if (!type)
            {
                return nullptr;
            }

            auto name = get_identifier();
            if (!name)
            {
                return nullptr;
            }

            if (!eat(token_type_t::equal))
            {
                raise_error();
                return nullptr;
            }

            auto infix = get_infix_node();
            if (!infix)
            {
                return nullptr;
            }

            return std::make_unique<declaration_node>(*type, name->str(), std::move(infix));
        }
    }
}
