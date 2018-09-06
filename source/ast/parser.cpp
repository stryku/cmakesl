#include "ast/parser.hpp"
#include "ast/ast_context.hpp"
#include "ast/type.hpp"
#include "ast/infix_node.hpp"
#include "ast/block_node.hpp"
#include "ast/user_function_node.hpp"
#include "ast/return_node.hpp"
#include "ast/infix_node.hpp"
#include "ast/declaration_node.hpp"
#include "ast/class_node.hpp"
#include "ast/member_declaration.hpp"
#include "ast/if_else_node.hpp"
#include "ast/conditional_node.hpp"
#include "ast/while_node.hpp"
#include "ast/class_builder.hpp"
#include "ast/list_type.hpp"
#include "ast/generic_type_factory.hpp"

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

        bool parser::eat_simple_type()
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
                token_type_t::kw_int,
                token_type_t::kw_double,
                token_type_t::kw_bool,
                token_type_t::kw_string,
                token_type_t::kw_list
            };

            return cmsl::contains(builtin_types, token_type);
        }

        const type* parser::get_type(ast_context& ctx) 
        {
            if(generic_type_starts(ctx))
            {
                return get_or_add_generic_type(ctx);
            }
            else
            {
                return get_simple_type(ctx);
            }
        }

        const type *parser::get_or_add_generic_type(ast_context &ctx)
        {
            const auto name_token = *m_token;

            if(!eat_simple_type())
            {
                return nullptr;
            }

            if(!eat(token_type_t::less))
            {
                return nullptr;
            }

            const auto value_type = get_type(ctx);

            if(!value_type)
            {
                return nullptr;
            }

            if(!eat(token_type_t::greater))
            {
                return nullptr;
            }

            const std::string type_name = name_token.str().to_string() + '<' + value_type->get_name() + '>';

            const auto found_type = ctx.find_type(type_name);
            if(found_type != nullptr)
            {
                return found_type;
            }

            auto t = generic_type_factory{}.create_list(type_name, ctx, *value_type);
            auto type_ptr = t.get();
            ctx.add_type(std::move(t));
            return type_ptr;
        }

        const type *parser::get_simple_type(ast_context &ctx)
        {
            const auto t = *m_token;

            if (!eat_simple_type())
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
            if(is_at_end())
            {
                return false;
            }

            return m_token->get_type() == token_type;
        }

        std::unique_ptr<infix_node> parser::get_infix()
        {
            token_container_t infix_tokens;

            while (current_is_infix_token())
            {
                infix_tokens.push_back(*m_token);
                eat();
            }

            if(infix_tokens.empty())
            {
                raise_error(); // todo raise missing infix expression error
                return nullptr;
            }

            if (!eat(token_type_t::semicolon))
            {
                return nullptr;
            }

            return std::make_unique<infix_node>(std::move(infix_tokens));
        }

        bool parser::current_is_type(ast_context& ctx) const
        {
            const auto token_type = m_token->get_type();
            if(is_builtin_type(token_type))
            {
                return true;
            }
            else if(token_type == token_type_t::identifier)
            {
                return ctx.find_type(m_token->str()) != nullptr;
            }
        }

        bool parser::current_is_infix_token() const
        {
            if (is_at_end())
            {
                return false;
            }

            const auto allowed_tokens = {
                token_type_t::integer,
                token_type_t::double_,
                token_type_t::dot,
                token_type_t::open_square,
                token_type_t::close_square,
                token_type_t::open_paren,
                token_type_t::close_paren,
                token_type_t::identifier,
                token_type_t::equal,
                token_type_t::equalequal,
                token_type_t::minus,
                token_type_t::minusminus,
                token_type_t::minusequal,
                token_type_t::plus,
                token_type_t::plusplus,
                token_type_t::plusequal,
                token_type_t::amp,
                token_type_t::ampamp,
                token_type_t::ampequal,
                token_type_t::pipe,
                token_type_t::pipepipe,
                token_type_t::pipeequal,
                token_type_t::slash,
                token_type_t::slashequal,
                token_type_t::star,
                token_type_t::starequal,
                token_type_t::percent,
                token_type_t::percentequal,
                token_type_t::exclaim,
                token_type_t::exclaimequal,
                token_type_t::xor_,
                token_type_t::xorequal,
                token_type_t::string,
                token_type_t::comma
            };

            return cmsl::contains(allowed_tokens, m_token->get_type());
        }

        std::unique_ptr<block_node> parser::get_block(ast_context& ctx)
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
                else if (declaration_starts(ctx))
                {
                    expr = get_declaration(ctx);
                }
                else if(current_is(token_type_t::kw_if))
                {
                    expr = get_if_else_node(ctx);
                }
                else if(current_is(token_type_t::kw_while))
                {
                    expr = get_while_node(ctx);
                }
                else
                {
                    expr = get_infix();
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

        std::unique_ptr<user_function_node> parser::get_function(ast_context& ctx)
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

            auto block_expr = get_block(ctx);

            if (!block_expr)
            {
                return nullptr;
            }

            return std::make_unique<user_function_node>(ctx, *type, name->str(), std::move(*parameters), std::move(block_expr));
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

            auto infix_expr = get_infix();
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


        bool parser::declaration_starts(ast_context& ctx) const
        {
            return current_is_type(ctx);
        }

        std::unique_ptr<declaration_node> parser::get_declaration(ast_context& ctx)
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

            std::unique_ptr<infix_node> infix;

            if (current_is(token_type_t::equal))
            {
                if (!eat(token_type_t::equal))
                {
                    raise_error();
                    return nullptr;
                }

                infix = get_infix();
                if (!infix)
                {
                    return nullptr;
                }
            }
            else
            {
                if (!eat(token_type_t::semicolon))
                {
                    return nullptr;
                }
            }

            return std::make_unique<declaration_node>(*type, name->str(), std::move(infix));
        }

        std::unique_ptr<class_node> parser::get_class(ast_context& ctx)
        {
            if (!eat(token_type_t::kw_class))
            {
                return nullptr;
            }

            auto name = get_identifier();
            if (!name)
            {
                return nullptr;
            }

            if (!eat(token_type_t::open_brace))
            {
                return nullptr;
            }

            class_builder builder{ ctx, name->str() };
            auto& class_ast_ctx = builder.get_ast_ctx();

            while (!current_is(token_type_t::close_brace))
            {
                if(class_function_starts())
                {
                    // pass context from builder
                    auto fun = get_function(ctx);
                    builder.with_function(std::move(fun));
                }
                else
                {
                    auto member = get_class_member_declaration(ctx);
                    if (!member)
                    {
                        return nullptr;
                    }

                    builder.with_member(std::move(*member));
                }
            }

            if (!eat(token_type_t::close_brace))
            {
                raise_error();
                return nullptr;
            }

            if (!eat(token_type_t::semicolon))
            {
                raise_error();
                return nullptr;
            }

            return builder.build();
        }

        bool parser::class_function_starts() const
        {
            // return_type name (
            //           ^
            return peek(2u) == token_type_t::open_paren;
        }

        boost::optional<member_declaration> parser::get_class_member_declaration(ast_context& ctx)
        {
            auto type = get_type(ctx);
            if (!type)
            {
                return boost::none;
            }

            auto name = get_identifier();
            if (!name)
            {
                return boost::none;
            }

            std::unique_ptr<infix_node> init;

            if (!current_is(token_type_t::semicolon))
            {
                if (!eat(token_type_t::equal))
                {
                    return boost::none;
                }

                init = get_infix();
                if (!init)
                {
                    return boost::none;
                }
            }
            else if (!eat(token_type_t::semicolon))
            {
                return boost::none;
            }

            return member_declaration{ type, *name, std::move(init) };
        }

        std::unique_ptr<class_node> parser::try_get_class_node(ast_context& ctx)
        {
            return current_is(token_type_t::kw_class)
                ? get_class(ctx)
                : nullptr;
        }

        std::unique_ptr<if_else_node> parser::get_if_else_node(ast_context& ctx)
        {
            if(!expect_token(token_type_t::kw_if))
            {
                return nullptr;
            }

            std::vector<std::unique_ptr<conditional_node>> ifs;

            while(current_is(token_type_t::kw_if))
            {
                if(!eat(token_type_t::kw_if))
                {
                    return nullptr;
                }

                auto if_node = get_conditional_node(ctx);
                if(!if_node)
                {
                    return nullptr;
                }

                ifs.emplace_back(std::move(if_node));

                if(current_is(token_type_t::kw_else))
                {
                    const auto next_type = peek(1);
                    if(next_type == token_type_t::kw_if)
                    {
                        eat(token_type_t::kw_else);
                    }
                }
            }

            std::unique_ptr<block_node> else_node;
            if(current_is(token_type_t::kw_else))
            {
                eat(token_type_t::kw_else);
                else_node = get_block(ctx);
            }

            return std::make_unique<if_else_node>(std::move(ifs), std::move(else_node));
        }

        boost::optional<parser::token_it> parser::find_matching_close_paren(const std::vector<parser::token_type_t> &forbidden_tokens) const
        {
            // todo handle comments

            // Assume starting on open paren that we want to match.
            int counter{};
            auto current = m_token;

            do
            {
                if(current == m_end)
                {
                    return boost::none;
                }

                const auto type = current->get_type();
                if(type == token_type_t::open_paren)
                {
                    ++counter;
                }
                else if(type == token_type_t::close_paren)
                {
                    --counter;
                }
                else if(contains(forbidden_tokens, type))
                {
                    return boost::none;
                }

                if(counter == 0)
                {
                    return current;
                }

                ++current;
            }while(counter != 0);

            return current;
        }

        std::unique_ptr<infix_node> parser::get_condition_infix_node()
        {
            if(!expect_token(token_type_t::open_paren))
            {
                return nullptr;
            }

            const auto forbidden_tokens = std::vector<parser::token_type_t>{
                    token_type_t::open_brace,
                    token_type_t::close_brace,
                    token_type_t::semicolon };

            auto condition_close_paren = find_matching_close_paren(forbidden_tokens);

            if(!condition_close_paren)
            {
                raise_error(); // todo raise paren match not found error
                return nullptr;
            }

            // m_token points at open paren, let's eat it
            eat(token_type_t::open_paren);

            auto condition_tokens = token_container_t{ m_token, *condition_close_paren };
            auto infix = std::make_unique<infix_node>(std::move(condition_tokens));

            // Point to token after the condition
            m_token = std::next(*condition_close_paren);

            return std::move(infix);
        }

        std::unique_ptr<while_node> parser::get_while_node(ast_context &ctx)
        {
            if(!eat(token_type_t::kw_while))
            {
                return nullptr;
            }

            auto conditional_node = get_conditional_node(ctx);
            if(!conditional_node)
            {
                return nullptr;
            }

            return std::make_unique<while_node>(std::move(conditional_node));
        }

        std::unique_ptr<conditional_node> parser::get_conditional_node(ast_context &ctx)
        {
            auto condition = get_condition_infix_node();
            if(!condition)
            {
                return nullptr;
            }

            auto block = get_block(ctx);
            if(!block)
            {
                return nullptr;
            }

            return std::make_unique<conditional_node>(std::move(condition), std::move(block));
        }

        bool parser::generic_type_starts(ast_context& ctx) const
        {
            return current_is_type(ctx) && peek(1u) == token_type_t::less;
        }
    }
}
