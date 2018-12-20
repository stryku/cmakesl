#include "ast/parser2.hpp"

#include "ast/variable_declaration_node.hpp"
#include "ast/infix_nodes.hpp"
#include "ast/return_node.hpp"
#include "ast/conditional_node.hpp"
#include "ast/if_else_node.hpp"
#include "ast/while_node.hpp"
#include "ast/user_function_node.hpp"
#include "ast/class_node.hpp"
#include "ast/translation_unit_node.hpp"


#include "common/algorithm.hpp"
#include "common/assert.hpp"

#include "errors/error.hpp"
#include "errors/errors_observer.hpp"

namespace cmsl
{
    namespace ast
    {
        parser2::parser2(errors::errors_observer& err_observer, const token_container_t& tokens)
                : m_err_observer{ err_observer }
                , m_token{ tokens.cbegin() }
                , m_end{ tokens.cend() }
        {}

        void parser2::raise_error()
        {
            m_err_observer.nofify_error(errors::error{});
        }

        std::unique_ptr<ast_node> parser2::translation_unit()
        {
            std::vector<std::unique_ptr<ast_node>> nodes;

            while(!is_at_end())
            {
                std::unique_ptr<ast_node> node;

                if(function_declaration_starts())
                {
                    node = function();
                }
                else if(current_is(token_type_t::kw_class))
                {
                    node = class_();
                }
                else
                {
                    node = variable_declaration();
                }

                if(!node)
                {
                    return nullptr;
                }

                nodes.emplace_back(std::move(node));
            }

            return std::make_unique<translation_unit_node>(std::move(nodes));
        }

        std::unique_ptr<ast_node> parser2::constructor(token_t class_name)
        {
            auto type_name = eat(token_type_t::identifier);
            if(!type_name)
            {
                return nullptr;
            }

            auto parameters = param_declarations();
            if (!parameters)
            {
                return nullptr;
            }

            auto block_expr = block();
            if (!block_expr)
            {
                return nullptr;
            }

            // Let's pretend that ctors return something.
            const auto return_type_reference = type_reference{class_name, class_name};
            return std::make_unique<user_function_node2>(return_type_reference,
                                                        *type_name,
                                                        std::move(*parameters),
                                                        std::move(block_expr));
        }

        std::unique_ptr<ast_node> parser2::class_()
        {
            if (!eat(token_type_t::kw_class))
            {
                return nullptr;
            }

            auto name = eat(token_type_t::identifier);
            if (!name)
            {
                return nullptr;
            }

            if (!eat(token_type_t::open_brace))
            {
                return nullptr;
            }

            std::vector<std::unique_ptr<ast_node>> class_nodes;

            while (!current_is(token_type_t::close_brace))
            {
                const auto class_ctor_starts = m_token->str() == name->str();
                if(class_ctor_starts)
                {
                    auto ctor = constructor(*name);
                    if(!ctor)
                    {
                        return nullptr;
                    }
                    class_nodes.emplace_back(std::move(ctor));
                }
                if(function_declaration_starts())
                {
                    auto fun = function();
                    if(!fun)
                    {
                        return nullptr;
                    }

                    class_nodes.emplace_back(std::move(fun));
                }
                else
                {
                    auto member = variable_declaration();
                    if (!member)
                    {
                        return nullptr;
                    }

                    class_nodes.emplace_back(std::move(member));
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

            return std::make_unique<class_node2>(*name, std::move(class_nodes));
        }

        std::unique_ptr<ast_node> parser2::function()
        {
            const auto ty = type();
            if(!ty)
            {
                return nullptr;
            }

            const auto name = eat(token_type_t::identifier);
            if(!name)
            {
                return nullptr;
            }

            auto parameters = param_declarations();
            if (!parameters)
            {
                return nullptr;
            }

            auto block_node = block();

            if (!block_node)
            {
                return nullptr;
            }

            return std::make_unique<user_function_node2>(*ty, *name, std::move(*parameters), std::move(block_node));
        }

        std::unique_ptr<ast_node> parser2::get_return_node()
        {
            if (!eat(token_type_t::kw_return))
            {
                return nullptr;
            }

            auto e = expr();
            if (!e)
            {
                return nullptr;
            }

            return std::make_unique<return_node>(std::move(e));
        }

        bool parser2::current_is_type() const
        {
            return is_builtin_simple_type(curr_type()) || current_is(token_type_t::identifier);
        }

        bool parser2::declaration_starts() const
        {
            return current_is_type();
        }

        std::unique_ptr<conditional_node> parser2::get_conditional_node()
        {
            if(!eat(token_type_t::open_paren))
            {
                return nullptr;
            }

            auto condition = expr();
            if(!condition)
            {
                return nullptr;
            }

            if(!eat(token_type_t::close_paren))
            {
                return nullptr;
            }

            auto b = block();
            if(!b)
            {
                return nullptr;
            }


            return std::make_unique<conditional_node>(std::move(condition), std::move(b));
        }

        std::unique_ptr<ast_node> parser2::get_if_else_node()
        {
            if(!current_is(token_type_t::kw_if))
            {
                // Expected if
                raise_error();
                return nullptr;
            }

            std::vector<std::unique_ptr<conditional_node>> ifs;

            while(current_is(token_type_t::kw_if))
            {
                if(!eat(token_type_t::kw_if))
                {
                    return nullptr;
                }

                auto if_node = get_conditional_node();
                if(!if_node)
                {
                    return nullptr;
                }

                ifs.emplace_back(std::move(if_node));

                if(current_is(token_type_t::kw_else))
                {
                    const auto next_type = peek();
                    if(next_type == token_type_t::kw_if)
                    {
                        eat(token_type_t::kw_else);
                    }
                }
            }

            std::unique_ptr<block_node> else_block;
            if(current_is(token_type_t::kw_else))
            {
                eat(token_type_t::kw_else);
                else_block = block();
            }

            return std::make_unique<if_else_node>(std::move(ifs), std::move(else_block));
        }

        std::unique_ptr<ast_node> parser2::get_while_node()
        {
            if(!eat(token_type_t::kw_while))
            {
                return nullptr;
            }

            auto conditional_node = get_conditional_node();
            if(!conditional_node)
            {
                return nullptr;
            }

            return std::make_unique<while_node>(std::move(conditional_node));
        }

        std::unique_ptr<block_node> parser2::block()
        {
            if (!eat(token_type_t::open_brace))
            {
                return nullptr;
            }

            std::vector<std::unique_ptr<ast_node>> nodes;

            while (!is_at_end() && !current_is(token_type_t::close_brace))
            {
                std::unique_ptr<ast_node> node;

                if (current_is(token_type_t::kw_return))
                {
                    node = get_return_node();
                }
                else if (declaration_starts())
                {
                    node = variable_declaration();
                }
                else if(current_is(token_type_t::kw_if))
                {
                    node = get_if_else_node();
                }
                else if(current_is(token_type_t::kw_while))
                {
                    node = get_while_node();
                }
                else
                {
                    node = expr();
                }

                if (!node)
                {
                    return nullptr;
                }

                nodes.emplace_back(std::move(node));
            }

            if (!eat(token_type_t::close_brace))
            {
                return nullptr;
            }

            return std::make_unique<block_node>(std::move(nodes));
        }

        boost::optional<param_declaration> parser2::get_param_declaration()
        {
            auto t = type();

            if (!t)
            {
                return{};
            }

            auto name = eat(token_type_t::identifier);
            if (!name)
            {
                return{};
            }

            return param_declaration{ *t, *name };
        }

        bool parser2::prepare_for_next_parameter_declaration()
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

            if (!is_at_end() && current_is(token_type_t::close_paren))
            {
                // Missed last parameter declaration
                raise_error();
                return false;
            }

            return true;
        }

        boost::optional<std::vector<param_declaration>> parser2::param_declarations()
        {
            std::vector<param_declaration> params;

            if (!eat(token_type_t::open_paren))
            {
                return {};
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

                const auto param_decl = get_param_declaration();
                if (!param_decl)
                {
                    return {};
                }

                params.push_back(std::move(*param_decl));

                if (!prepare_for_next_parameter_declaration())
                {
                    return {};
                }
            }

            if(!eat(token_type_t::close_paren))
            {
                return {};
            }

            return params;
        }

        std::unique_ptr<ast_node> parser2::variable_declaration()
        {
            const auto ty = type();
            if(!ty)
            {
                return nullptr;
            }

            const auto name = eat(token_type_t::identifier);
            if(!name)
            {
                return nullptr;
            }

            std::unique_ptr<ast_node> initialization;

            if(current_is(token_type_t::equal))
            {
                eat(); // equal
                initialization = expr();
                if(!initialization)
                {
                    // todo expected expression
                    raise_error();
                    return nullptr;
                }
            }

            return std::make_unique<variable_declaration_node>(*ty, *name, std::move(initialization));
        }

        boost::optional<type_reference> parser2::type()
        {
            if(generic_type_starts())
            {
                return generic_type();
            }
            else
            {
                return simple_type();
            }
        }

        boost::optional<parser2::token_t> parser2::eat_simple_type()
        {
            const auto token_type = curr_type();

            if (is_builtin_simple_type(token_type) ||
                token_type == token_type_t::identifier)
            {
                return eat();
            }
            else
            {
                raise_error();
                return {};
            }
        }

        bool parser2::is_builtin_simple_type(token_type_t token_type) const
        {
            const auto simple_types = {
                    token_type_t::kw_int,
                    token_type_t::kw_double,
                    token_type_t::kw_bool,
                    token_type_t::kw_string
            };

            return cmsl::contains(simple_types, token_type);
        }

        boost::optional<type_reference> parser2::simple_type()
        {
            const auto type_token = *m_token;

            if (!eat_simple_type())
            {
                return {};
            }

            return type_reference{type_token, type_token};
        }

        boost::optional<type_reference> parser2::generic_type()
        {
            const auto name_token = *m_token;

            if(!eat_generic_type())
            {
                return {};
            }

            if(!eat(token_type_t::less))
            {
                return {};
            }

            const auto value_type = type();

            if(!value_type)
            {
                return {};
            }

            if(is_at_end())
            {
                raise_error();
                return {};
            }

            const auto closing_greater_token = *m_token;

            if(!eat(token_type_t::greater))
            {
                return {};
            }

            return type_reference{name_token, closing_greater_token};
        }

        bool parser2::generic_type_starts() const
        {
            return current_is_generic_type() && peek() == token_type_t::less;
        }

        parser2::token_type_t parser2::peek(size_t n) const
        {
            if (std::distance(m_token, m_end) <= n)
            {
                return token_type_t::undef;
            }

            return std::next(m_token, n)->get_type();
        }

        bool parser2::current_is_generic_type() const
        {
            const auto generic_types = {
                    token_type_t::kw_list
            };

            return cmsl::contains(generic_types, curr_type());
        }

        boost::optional<parser2::token_t> parser2::eat_generic_type()
        {
            if(!current_is_generic_type())
            {
                raise_error();
                return {};
            }

            return eat();
        }

        bool parser2::expect_not_at_end()
        {
            if (is_at_end())
            {
                raise_error();
                return false;
            }

            return true;
        }

        boost::optional<parser2::token_t> parser2::eat(boost::optional<token_type_t> type)
        {
            if (!expect_not_at_end())
            {
                return {};
            }

            if (type && !current_is(*type))
            {
                raise_error();
                return {};
            }

            const auto t = *m_token;
            ++m_token;
            return t;
        }

        bool parser2::current_is(token_type_t token_type) const
        {
            if(is_at_end())
            {
                return false;
            }

            return curr_type() == token_type;
        }

        bool parser2::is_at_end() const
        {
            return m_token == m_end;
        }

        bool parser2::next_is(token_type_t token_type) const
        {
            return peek() == token_type;
        }

        bool parser2::is_current_operator_2() const
        {
            return curr_type() == token_type_t::dot;
        }

        bool parser2::is_current_operator_3() const
        {
            return curr_type() == token_type_t::exclaim;
        }

        bool parser2::is_current_operator_5() const
        {
            const auto operators = {token_type_t::star, token_type_t::slash, token_type_t::percent};
            return contains(operators, curr_type());
        }

        bool parser2::is_current_operator_6() const
        {
            const auto operators = {token_type_t::plus, token_type_t::minus};
            return contains(operators, curr_type());
        }

        bool parser2::is_current_operator_9() const
        {
            const auto operators = {
                    token_type_t::less,
                    token_type_t::lessequal,
                    token_type_t::greater,
                    token_type_t::greaterequal
            };
            return contains(operators, curr_type());
        }

        bool parser2::is_current_operator_10() const
        {
            const auto operators = {
                    token_type_t::equalequal,
                    token_type_t::exclaimequal
            };
            return contains(operators, curr_type());
        }

        bool parser2::is_current_operator_11() const
        {
            return curr_type() == token_type_t::amp;
        }

        bool parser2::is_current_operator_12() const
        {
            return curr_type() == token_type_t::xor_;
        }

        bool parser2::is_current_operator_13() const
        {
            return curr_type() == token_type_t::pipe;
        }

        bool parser2::is_current_operator_14() const
        {
            return curr_type() == token_type_t::ampamp;
        }

        bool parser2::is_current_operator_15() const
        {
            return curr_type() == token_type_t::pipepipe;
        }

        bool parser2::is_current_operator_16() const
        {
            const auto operators = {
                    token_type_t::equal,
                    token_type_t::plusequal,
                    token_type_t::minusequal,
                    token_type_t::starequal,
                    token_type_t::slashequal,
                    token_type_t::ampequal,
                    token_type_t::xorequal,
                    token_type_t::pipeequal
            };
            return contains(operators, curr_type());
        }

        std::unique_ptr<ast_node> parser2::operator_2()
        {
            auto f = factor();

            while(!is_at_end() &&
                  is_current_operator_2())
            {
                eat(); // dot
                auto lhs = std::move(f);
                if(current_is_class_member_access())
                {
                    const auto member_name = eat();
                    f = std::make_unique<class_member_access_node>(std::move(lhs),
                                                                   *member_name);
                }
                else // class member function call
                {
                    auto vals = get_function_call_values();
                    if(!vals)
                    {
                        return nullptr;
                    }

                    f = std::make_unique<member_function_call_node>(std::move(lhs),
                                                                    vals->name,
                                                                    std::move(vals->params));
                }
            }

            return std::move(f);
        }

        std::unique_ptr<ast_node> parser2::operator_3()
        {
            auto f = operator_2();

            while(!is_at_end() &&
                  is_current_operator_3())
            {
                const auto op = *m_token;
                eat(); // eat operator

                auto rhs = operator_2();

                auto lhs = std::move(f);
                f = std::make_unique<binary_operator_node>(std::move(lhs), op, std::move(rhs));
            }

            return std::move(f);
        }

        std::unique_ptr<ast_node> parser2::operator_5()
        {
            auto f = operator_3();

            while(!is_at_end() &&
                  is_current_operator_5())
            {
                const auto op = *m_token;
                eat(); // eat operator

                auto rhs = operator_3();

                auto lhs = std::move(f);
                f = std::make_unique<binary_operator_node>(std::move(lhs), op, std::move(rhs));
            }

            return std::move(f);
        }

        std::unique_ptr<ast_node> parser2::operator_6()
        {
            auto f = operator_5();

            while(!is_at_end() &&
                  is_current_operator_6())
            {
                const auto op = *m_token;
                eat(); // eat operator

                auto rhs = operator_5();

                auto lhs = std::move(f);
                f = std::make_unique<binary_operator_node>(std::move(lhs), op, std::move(rhs));
            }

            return std::move(f);
        }

        std::unique_ptr<ast_node> parser2::operator_9()
        {
            auto f = operator_6();

            while(!is_at_end() &&
                  is_current_operator_9())
            {
                const auto op = *m_token;
                eat(); // eat operator

                auto rhs = operator_6();

                auto lhs = std::move(f);
                f = std::make_unique<binary_operator_node>(std::move(lhs), op, std::move(rhs));
            }

            return std::move(f);
        }

        std::unique_ptr<ast_node> parser2::operator_10()
        {
            auto f = operator_9();

            while(!is_at_end() &&
                  is_current_operator_10())
            {
                const auto op = *m_token;
                eat(); // eat operator

                auto rhs = operator_9();

                auto lhs = std::move(f);
                f = std::make_unique<binary_operator_node>(std::move(lhs), op, std::move(rhs));
            }

            return std::move(f);
        }

        std::unique_ptr<ast_node> parser2::operator_11()
        {
            auto f = operator_10();

            while(!is_at_end() &&
                  is_current_operator_11())
            {
                const auto op = *m_token;
                eat(); // eat operator

                auto rhs = operator_10();

                auto lhs = std::move(f);
                f = std::make_unique<binary_operator_node>(std::move(lhs), op, std::move(rhs));
            }

            return std::move(f);
        }

        std::unique_ptr<ast_node> parser2::operator_12()
        {
            auto f = operator_11();

            while(!is_at_end() &&
                  is_current_operator_12())
            {
                const auto op = *m_token;
                eat(); // eat operator

                auto rhs = operator_11();

                auto lhs = std::move(f);
                f = std::make_unique<binary_operator_node>(std::move(lhs), op, std::move(rhs));
            }

            return std::move(f);
        }

        std::unique_ptr<ast_node> parser2::operator_13()
        {
            auto f = operator_12();

            while(!is_at_end() &&
                  is_current_operator_13())
            {
                const auto op = *m_token;
                eat(); // eat operator

                auto rhs = operator_12();

                auto lhs = std::move(f);
                f = std::make_unique<binary_operator_node>(std::move(lhs), op, std::move(rhs));
            }

            return std::move(f);
        }

        std::unique_ptr<ast_node> parser2::operator_14()
        {
            auto f = operator_13();

            while(!is_at_end() &&
                  is_current_operator_14())
            {
                const auto op = *m_token;
                eat(); // eat operator

                auto rhs = operator_13();

                auto lhs = std::move(f);
                f = std::make_unique<binary_operator_node>(std::move(lhs), op, std::move(rhs));
            }

            return std::move(f);
        }

        std::unique_ptr<ast_node> parser2::operator_15()
        {
            auto f = operator_14();

            while(!is_at_end() &&
                  is_current_operator_15())
            {
                const auto op = *m_token;
                eat(); // eat operator

                auto rhs = operator_14();

                auto lhs = std::move(f);
                f = std::make_unique<binary_operator_node>(std::move(lhs), op, std::move(rhs));
            }

            return std::move(f);
        }

        std::unique_ptr<ast_node> parser2::operator_16()
        {
            auto f = operator_15();

            while(!is_at_end() &&
                  is_current_operator_16())
            {
                const auto op = *m_token;
                eat(); // eat operator

                auto rhs = operator_15();

                auto lhs = std::move(f);
                f = std::make_unique<binary_operator_node>(std::move(lhs), op, std::move(rhs));
            }

            return std::move(f);
        }

        parser2::token_type_t parser2::curr_type() const
        {
            return is_at_end() ? token_type_t::undef : m_token->get_type();
        }

        bool parser2::current_is_class_member_access() const
        {
            // At this point we know that we're after the dot token. Test whether it's a member function call or just member access.
            return !current_is_function_call();
        }

        bool parser2::current_is_function_call() const
        {
            return curr_type() == token_type_t::identifier
                   && next_is(token_type_t::open_paren);
        }

        bool parser2::current_is_fundamental_value() const
        {
            switch(curr_type())
            {
                case lexer::token::token_type::integer:
                case lexer::token::token_type::double_:
                case lexer::token::token_type::string:
                case lexer::token::token_type::kw_true:
                case lexer::token::token_type::kw_false:
                    return true;

                default:
                    return false;
            }
        }

        std::unique_ptr<ast_node> parser2::fundamental_value()
        {
            const auto token = eat();

            if(!token)
            {
                return nullptr;
            }

            switch(token->get_type())
            {
                case lexer::token::token_type::integer:
                    return std::make_unique<int_value_node>(*token);

                case lexer::token::token_type::double_:
                    return std::make_unique<double_value_node>(*token);

                case lexer::token::token_type::string:
                    return std::make_unique<string_value_node>(*token);

                case lexer::token::token_type::kw_true:
                case lexer::token::token_type::kw_false:
                    return std::make_unique<bool_value_node>(*token);

                default:
                    return nullptr;
            }
        }

        std::unique_ptr<ast_node> parser2::factor()
        {
            if(current_is_fundamental_value())
            {
                return fundamental_value();
            }
            else if(current_is(token_type_t::open_paren))
            {
                eat(token_type_t::open_paren);
                auto e = expr();
                eat(token_type_t::close_paren);
                return std::move(e);
            }
            else if(current_is(token_type_t::identifier))
            {
                const auto id = eat(token_type_t::identifier);
                return id ? std::make_unique<id_node>(*id) : nullptr;
            }

            CMSL_UNREACHABLE("Can not create a factor");
            return nullptr;
        }

        std::unique_ptr<ast_node> parser2::expr()
        {
            if(current_is_function_call())
            {
                return function_call();
            }
            else
            {
                auto operator_expr = operator_16();

                if(current_is(token_type_t::dot))
                {
                    eat(token_type_t::dot);
                    if(current_is_class_member_access())
                    {
                        const auto member_name = eat();
                        return std::make_unique<class_member_access_node>(std::move(operator_expr),
                                                                          *member_name);
                    }
                    else // class member function call
                    {
                        auto vals = get_function_call_values();

                        if(!vals)
                        {
                            return nullptr;
                        }

                        return std::make_unique<member_function_call_node>(std::move(operator_expr),
                                                                           vals->name,
                                                                           std::move(vals->params));
                    }
                }

                return std::move(operator_expr);
            }
        }
        boost::optional<parser2::function_call_values> parser2::get_function_call_values()
        {
            function_call_values vals;

            vals.name = *eat(token_type_t::identifier);
            auto params = parameter_list();
            if(!params)
            {
                return {};
            }

            vals.params = std::move(*params);

            return std::move(vals);
        }

        boost::optional<std::vector<std::unique_ptr<ast_node>>> parser2::parameter_list()
        {
            eat(token_type_t::open_paren);

            std::vector<std::unique_ptr<ast_node>> params;

            while(!is_at_end() && !current_is(token_type_t::close_paren))
            {
                auto param = expr();

                if(!param)
                {
                    // Todo: Unexpected token e.g. semicolon
                    raise_error();
                    return {};
                }

                params.emplace_back(std::move(param));

                if(current_is(token_type_t::comma))
                {
                    eat(token_type_t::comma);
                }
            }

            eat(token_type_t::close_paren);

            return std::move(params);
        }

        std::unique_ptr<ast_node> parser2::function_call()
        {
            auto vals = get_function_call_values();
            if(!vals)
            {
                return nullptr;
            }

            return std::make_unique<function_call_node>(vals->name, std::move(vals->params));
        }

        bool parser2::function_declaration_starts() const
        {
            // Check if there is an open paren
            // return_type function_name (
            return peek(2u) == token_type_t::open_paren;
        }
    }
}
