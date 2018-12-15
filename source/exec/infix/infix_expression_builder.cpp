#include "exec/infix/infix_expression_builder.hpp"


#include "exec/infix/binary_operator_expression.hpp"
#include "exec/infix/fundamental_value_expression.hpp"
#include "exec/infix/id_expression.hpp"
#include "exec/infix/function_call_expression.hpp"
#include "exec/infix/member_function_call_expression.hpp"
#include "exec/infix/class_member_access_expression.hpp"


namespace cmsl
{
    namespace exec
    {
        namespace infix
        {
            infix_expression_builder::infix_expression_builder(function_caller& fun_caller, const tokens_t &tokens)
                : m_token{ tokens.cbegin() }
                , m_end{ tokens.cend() }
                , m_fun_caller{ fun_caller }
            {}

            std::unique_ptr<infix_expression> infix_expression_builder::build()
            {
                return expr();
            }

            std::unique_ptr<infix_expression> infix_expression_builder::factor()
            {
                const auto type = curr_type();

                if(is_current_fundamental_value())
                {
                    return fundamental_value();
                }
                else if(type == token_type_t::open_paren)
                {
                    eat();
                    auto e = expr();
                    eat(token_type_t::close_paren);
                    return std::move(e);
                }
                else if(type == token_type_t::identifier)
                {
                    auto e = std::make_unique<id_expression>(*m_token);
                    eat();
                    return std::move(e);
                }
            }

            infix_expression_builder::token_type_t infix_expression_builder::curr_type() const
            {
                return m_token->get_type();
            }

            bool infix_expression_builder::is_current_fundamental_value() const
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

            std::unique_ptr<infix_expression> infix_expression_builder::fundamental_value()
            {
                auto expr = make_fundamental_expression(*m_token);
                eat();
                return std::move(expr);
            }

            void infix_expression_builder::eat()
            {
                ++m_token;
            }

            bool infix_expression_builder::eat(infix_expression_builder::token_type_t expected_type)
            {
                if(!expect_token(expected_type))
                {
                    return false;
                }

                eat();
                return true;
            }

            bool infix_expression_builder::is_at_end() const
            {
                return m_token == m_end;
            }

            bool infix_expression_builder::expect_token(infix_expression_builder::token_type_t expected_type) const
            {
                return !is_at_end() && m_token->get_type() == expected_type;
            }

            std::unique_ptr<infix_expression> infix_expression_builder::expr()
            {
                if(is_current_function_call())
                {
                    return function_call();
                }
                else
                {
                    auto operator_expr = operator_expr_16();

                    if(!is_at_end() && curr_type() == token_type_t::dot)
                    {
                        eat();
                        if(is_current_class_member_access())
                        {
                            const auto member_name = *m_token;
                            eat();
                            return std::make_unique<class_member_access_expression>(std::move(operator_expr),
                                                                                    member_name);
                        }
                        else
                        {
                            auto vals = get_function_call_values();

                            return std::make_unique<member_function_call_expression>(m_fun_caller,
                                                                                     std::move(operator_expr),
                                                                                     vals.name,
                                                                                     std::move(vals.params));
                        }
                    }

                    return std::move(operator_expr);
                }
            }

            std::unique_ptr<infix_expression> infix_expression_builder::function_call()
            {
                auto vals = get_function_call_values();
                return std::make_unique<function_call_expression>(m_fun_caller, vals.name, std::move(vals.params));
            }

            bool infix_expression_builder::is_current_function_call() const
            {
                return curr_type() == token_type_t::identifier
                        && is_next(token_type_t::open_paren);
            }

            bool infix_expression_builder::is_current_class_member_access() const
            {
                // At this point we know that we're after the dot token. Test whether it's a member function call or just member access.
                return !is_current_function_call();
            }

            bool infix_expression_builder::is_next(infix_expression_builder::token_type_t expected_type) const
            {
                return !is_at_end() && peek(1) == expected_type;
            }

            infix_expression_builder::token_type_t infix_expression_builder::peek(size_t n) const
            {
                if(std::distance(m_token, m_end) <= n)
                {
                    return token_type_t::undef;
                }

                return std::next(m_token, n)->get_type();
            }

            infix_expression_builder::function_call_values infix_expression_builder::get_function_call_values()
            {
                function_call_values vals;

                vals.name = *m_token;
                eat(token_type_t::identifier);

                vals.params = parameter_list();

                return std::move(vals);
            }

            std::vector<std::unique_ptr<infix_expression>> infix_expression_builder::parameter_list()
            {
                eat(token_type_t::open_paren);

                std::vector<std::unique_ptr<infix_expression>> params;

                while(curr_type() != token_type_t::close_paren)
                {
                    auto param = expr();
                    params.emplace_back(std::move(param));

                    if(curr_type() == token_type_t::comma)
                    {
                        eat();
                    }
                }

                eat(token_type_t::close_paren);

                return std::move(params);
            }

            bool infix_expression_builder::is_current_operator_2() const
            {
                return curr_type() == token_type_t::dot;
            }

            bool infix_expression_builder::is_current_operator_3() const
            {
                return curr_type() == token_type_t::exclaim;
            }

            bool infix_expression_builder::is_current_operator_5() const
            {
                const auto operators = {token_type_t::star, token_type_t::slash, token_type_t::percent};
                return contains(operators, curr_type());
            }

            bool infix_expression_builder::is_current_operator_6() const
            {
                const auto operators = {token_type_t::plus, token_type_t::minus};
                return contains(operators, curr_type());
            }

            bool infix_expression_builder::is_current_operator_9() const
            {
                const auto operators = {
                        token_type_t::less,
                        token_type_t::lessequal,
                        token_type_t::greater,
                        token_type_t::greaterequal
                };
                return contains(operators, curr_type());
            }

            bool infix_expression_builder::is_current_operator_10() const
            {
                const auto operators = {
                        token_type_t::equalequal,
                        token_type_t::exclaimequal
                };
                return contains(operators, curr_type());
            }

            bool infix_expression_builder::is_current_operator_11() const
            {
                return curr_type() == token_type_t::amp;
            }

            bool infix_expression_builder::is_current_operator_12() const
            {
                return curr_type() == token_type_t::xor_;
            }

            bool infix_expression_builder::is_current_operator_13() const
            {
                return curr_type() == token_type_t::pipe;
            }

            bool infix_expression_builder::is_current_operator_14() const
            {
                return curr_type() == token_type_t::ampamp;
            }

            bool infix_expression_builder::is_current_operator_15() const
            {
                return curr_type() == token_type_t::pipepipe;
            }

            bool infix_expression_builder::is_current_operator_16() const
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

            std::unique_ptr<infix_expression> infix_expression_builder::operator_expr_2()
            {
                auto f = factor();

                while(!is_at_end() &&
                        is_current_operator_2())
                {
                    eat(); // dot
                    auto lhs = std::move(f);
                    if(is_current_class_member_access())
                    {
                        const auto member_name = *m_token;
                        eat();
                        f = std::make_unique<class_member_access_expression>(std::move(lhs),
                                                                                member_name);
                    }
                    else
                    {
                        auto vals = get_function_call_values();
                        f = std::make_unique<member_function_call_expression>(m_fun_caller,
                                                                                   std::move(lhs),
                                                                                   vals.name,
                                                                                   std::move(vals.params));
                    }
                }

                return std::move(f);
            }

            std::unique_ptr<infix_expression> infix_expression_builder::operator_expr_3()
            {
                auto f = operator_expr_2();

                while(!is_at_end() &&
                      is_current_operator_3())
                {
                    const auto op = *m_token;
                    eat(); // eat operator

                    auto rhs = operator_expr_2();

                    auto lhs = std::move(f);
                    f = std::make_unique<binary_operator_expression>(std::move(lhs), op, std::move(rhs));
                }

                return std::move(f);
            }

            std::unique_ptr<infix_expression> infix_expression_builder::operator_expr_5()
            {
                auto f = operator_expr_3();

                while(!is_at_end() &&
                      is_current_operator_5())
                {
                    const auto op = *m_token;
                    eat(); // eat operator

                    auto rhs = operator_expr_3();

                    auto lhs = std::move(f);
                    f = std::make_unique<binary_operator_expression>(std::move(lhs), op, std::move(rhs));
                }

                return std::move(f);
            }

            std::unique_ptr<infix_expression> infix_expression_builder::operator_expr_6()
            {
                auto f = operator_expr_5();

                while(!is_at_end() &&
                      is_current_operator_6())
                {
                    const auto op = *m_token;
                    eat(); // eat operator

                    auto rhs = operator_expr_5();

                    auto lhs = std::move(f);
                    f = std::make_unique<binary_operator_expression>(std::move(lhs), op, std::move(rhs));
                }

                return std::move(f);
            }

            std::unique_ptr<infix_expression> infix_expression_builder::operator_expr_9()
            {
                auto f = operator_expr_6();

                while(!is_at_end() &&
                      is_current_operator_9())
                {
                    const auto op = *m_token;
                    eat(); // eat operator

                    auto rhs = operator_expr_6();

                    auto lhs = std::move(f);
                    f = std::make_unique<binary_operator_expression>(std::move(lhs), op, std::move(rhs));
                }

                return std::move(f);
            }

            std::unique_ptr<infix_expression> infix_expression_builder::operator_expr_10()
            {
                auto f = operator_expr_9();

                while(!is_at_end() &&
                      is_current_operator_10())
                {
                    const auto op = *m_token;
                    eat(); // eat operator

                    auto rhs = operator_expr_9();

                    auto lhs = std::move(f);
                    f = std::make_unique<binary_operator_expression>(std::move(lhs), op, std::move(rhs));
                }

                return std::move(f);
            }

            std::unique_ptr<infix_expression> infix_expression_builder::operator_expr_11()
            {
                auto f = operator_expr_10();

                while(!is_at_end() &&
                      is_current_operator_11())
                {
                    const auto op = *m_token;
                    eat(); // eat operator

                    auto rhs = operator_expr_10();

                    auto lhs = std::move(f);
                    f = std::make_unique<binary_operator_expression>(std::move(lhs), op, std::move(rhs));
                }

                return std::move(f);
            }

            std::unique_ptr<infix_expression> infix_expression_builder::operator_expr_12()
            {
                auto f = operator_expr_11();

                while(!is_at_end() &&
                      is_current_operator_12())
                {
                    const auto op = *m_token;
                    eat(); // eat operator

                    auto rhs = operator_expr_11();

                    auto lhs = std::move(f);
                    f = std::make_unique<binary_operator_expression>(std::move(lhs), op, std::move(rhs));
                }

                return std::move(f);
            }

            std::unique_ptr<infix_expression> infix_expression_builder::operator_expr_13()
            {
                auto f = operator_expr_12();

                while(!is_at_end() &&
                      is_current_operator_13())
                {
                    const auto op = *m_token;
                    eat(); // eat operator

                    auto rhs = operator_expr_12();

                    auto lhs = std::move(f);
                    f = std::make_unique<binary_operator_expression>(std::move(lhs), op, std::move(rhs));
                }

                return std::move(f);
            }

            std::unique_ptr<infix_expression> infix_expression_builder::operator_expr_14()
            {
                auto f = operator_expr_13();

                while(!is_at_end() &&
                      is_current_operator_14())
                {
                    const auto op = *m_token;
                    eat(); // eat operator

                    auto rhs = operator_expr_13();

                    auto lhs = std::move(f);
                    f = std::make_unique<binary_operator_expression>(std::move(lhs), op, std::move(rhs));
                }

                return std::move(f);
            }

            std::unique_ptr<infix_expression> infix_expression_builder::operator_expr_15()
            {
                auto f = operator_expr_14();

                while(!is_at_end() &&
                      is_current_operator_15())
                {
                    const auto op = *m_token;
                    eat(); // eat operator

                    auto rhs = operator_expr_14();

                    auto lhs = std::move(f);
                    f = std::make_unique<binary_operator_expression>(std::move(lhs), op, std::move(rhs));
                }

                return std::move(f);
            }

            std::unique_ptr<infix_expression> infix_expression_builder::operator_expr_16()
            {
                auto f = operator_expr_15();

                while(!is_at_end() &&
                      is_current_operator_16())
                {
                    const auto op = *m_token;
                    eat(); // eat operator

                    auto rhs = operator_expr_15();

                    auto lhs = std::move(f);
                    f = std::make_unique<binary_operator_expression>(std::move(lhs), op, std::move(rhs));
                }

                return std::move(f);
            }
        }
    }
}
