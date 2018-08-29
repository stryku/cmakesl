#include "exec/onp/infix_to_onp.hpp"

#include "ast/ast_context.hpp"
#include "lexer/token/token.hpp"
#include "common/algorithm.hpp"

#include <stack>

namespace cmsl
{
    namespace exec
    {
        namespace onp
        {
            infix_to_onp::infix_to_onp(const tokens_container_t& infix_tokens, const ast::ast_context& ctx)
                : m_token{ std::cbegin(infix_tokens) }
                , m_end{ std::cend(infix_tokens) }
                , m_ast_ctx{ ctx }
                , m_op_precedences{ get_operator_precedences() }
            {}

            infix_to_onp::tokens_container_t infix_to_onp::convert()
            {
                while (m_token != m_end)
                {
                    handle_token(*m_token);
                    if (m_token != m_end)
                    {
                        ++m_token;
                    }
                }

                while (!m_stack.empty())
                {
                    const auto tok = m_stack.top();
                    m_stack.pop();
                    m_out.emplace_back(tok);
                }

                return m_out;
            }

            lexer::token::token infix_to_onp::get_out_back_and_pop()
            {
                const auto tok = m_out.back();
                m_out.pop_back();
                return tok;
            }

            infix_to_onp::tokens_container_t infix_to_onp::extract_access_tokens_from_out()
            {
                tokens_container_t access_tokens;

                // Access tokens in onp are e.g.
                // id id .
                // id id . id .
                // id id . id . id . etc...

                // todo handle e.g. (4 + 2).to_string()


                // Gather them from back of out
                while(!m_out.empty())
                {
                    const auto& back_token = m_out.back();
                    if(back_token.get_type() == token_type_t::identifier)
                    {
                        access_tokens.push_back(get_out_back_and_pop());
                    }
                    else if (back_token.get_type() == token_type_t::dot)
                    {
                        access_tokens.push_back(get_out_back_and_pop());

                        // After dot there is always one or two identifier tokens. If there are two, this is the end of access tokens
                        access_tokens.push_back(get_out_back_and_pop());

                        if(m_out.back().get_type() == token_type_t::identifier)
                        {
                            // End of access tokens
                            access_tokens.push_back(get_out_back_and_pop());
                            break;
                        }
                    }
                    else
                    {
                        // If we are on constant or operator other than dot, that's the end of access tokens
                        break;
                    }
                }

                // Tokens popped from back are in reverse order
                std::reverse(std::begin(access_tokens), std::end(access_tokens));

                return access_tokens;
            }

            void infix_to_onp::convert_function_call()
            {
                const auto fun = *m_token++;
                m_out.push_back(fun);
                if(top_is(token_type_t::dot))
                {
                    m_out.push_back(get_top_and_pop());
                }

                const auto function_access_tokens = extract_access_tokens_from_out();

                if (!eat(token_type_t::open_paren))
                {
                    // todo raise error
                }

                // Threat every parameter as separate infix expression
                while (m_token != m_end && m_token->get_type() != token_type_t::close_paren)
                {
                    tokens_container_t param_tokens;
                    while(m_token != m_end
                          && m_token->get_type() != token_type_t::close_paren)
                    {
                        if(m_token->get_type() == token_type_t::comma)
                        {
                            // End of parameter tokens
                            ++m_token;
                            break;
                        }
                        else
                        {
                            param_tokens.push_back(*m_token++);
                        }
                    }

                    auto inf_to_onp = infix_to_onp{ param_tokens, m_ast_ctx };
                    auto param_onp_tokens = inf_to_onp.convert();
                    copy_tokens_to_out(param_onp_tokens);
                }

                copy_tokens_to_out(function_access_tokens);

                if (!eat(token_type_t::close_paren))
                {
                    // todo raise error
                }
            }

            void infix_to_onp::copy_tokens_to_out(const tokens_container_t& tokens)
            {
                std::copy(std::cbegin(tokens), std::cend(tokens),
                          std::back_inserter(m_out));
            }

            void infix_to_onp::handle_token(const lexer::token::token& token)
            {
                const auto token_type = token.get_type();

                if (token_type == token_type_t::integer)
                {
                    m_out.emplace_back(token);
                }
                else if (token_type == token_type_t::semicolon)
                {
                    // do nothing
                }
                else if (token_type == token_type_t::identifier)
                {
                    if(is_next(token_type_t::open_paren))
                    {
                        // Assume function call
                        convert_function_call();
                    }
                    else
                    {
                        // Assume variable access
                        m_out.emplace_back(token);
                    }
                }
                else if (is_operator(token_type))
                {
                    const auto o1 = token_type;

                    if (!m_stack.empty())
                    {
                        auto o2 = m_stack.top().get_type();

                        while (m_op_precedences[o1] >= m_op_precedences[o2])
                        {
                            m_out.emplace_back(get_top_and_pop());
                            if (m_stack.empty())
                            {
                                break;
                            }
                            o2 = m_stack.top().get_type();
                        }
                    }

                    m_stack.push(token);
                }
                else if (token_type == token_type_t::open_paren)
                {
                    m_stack.push(token);
                }
                else if (token_type == token_type_t::close_paren)
                {
                    while (!m_stack.empty() && m_stack.top().get_type() != token_type_t::open_paren)
                    {
                        const auto op = m_stack.top();
                        m_stack.pop();
                        m_out.emplace_back(op);
                    }

                    m_stack.pop();
                }
            }

            bool infix_to_onp::eat(token_type_t token_type)
            {
                if (m_token == m_end || m_token->get_type() != token_type)
                {
                    return false;
                }

                ++m_token;
                return true;
            }

            bool infix_to_onp::is_operator(token_type_t token_type) const
            {
                const auto operators = {
                    token_type_t::dot,
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
                    token_type_t::less,
                    token_type_t::lessequal,
                    token_type_t::greater,
                    token_type_t::greaterequal,
                    token_type_t::comma,
                    // todo add rest of operators
                };

                return contains(operators, token_type);
            }

            infix_to_onp::operator_precedences_t infix_to_onp::get_operator_precedences() const
            {
                operator_precedences_t prec;

                // From https://en.cppreference.com/w/cpp/language/operator_precedence
                prec[token_type_t::dot] = 2;

                prec[token_type_t::exclaim] = 3;

                prec[token_type_t::star] = 5;
                prec[token_type_t::slash] = 5;
                prec[token_type_t::percent] = 5;

                prec[token_type_t::plus] = 6;
                prec[token_type_t::minus] = 6;

                prec[token_type_t::less] = 9;
                prec[token_type_t::lessequal] = 9;
                prec[token_type_t::greater] = 9;
                prec[token_type_t::greaterequal] = 9;

                prec[token_type_t::equalequal] = 10;
                prec[token_type_t::exclaimequal] = 10;

                prec[token_type_t::amp] = 11;
                prec[token_type_t::xor_] = 12;
                prec[token_type_t::pipe] = 13;
                prec[token_type_t::ampamp] = 14;
                prec[token_type_t::pipepipe] = 15;

                prec[token_type_t::equal] = 16;
                prec[token_type_t::plusequal] = 16;
                prec[token_type_t::minusequal] = 16;
                prec[token_type_t::starequal] = 16;
                prec[token_type_t::slashequal] = 16;
                prec[token_type_t::slashequal] = 16;
                prec[token_type_t::ampequal] = 16;
                prec[token_type_t::xorequal] = 16;
                prec[token_type_t::pipeequal] = 16;

                // todo add rest of operators

                return prec;
            }

            lexer::token::token infix_to_onp::get_top_and_pop()
            {
                assert(!m_stack.empty());

                auto val = m_stack.top();
                m_stack.pop();
                return val;
            }

            bool infix_to_onp::is_next(token_type_t token_type) const
            {
                auto next_it = std::next(m_token);
                return next_it != m_end && next_it->get_type() == token_type;
            }

            bool infix_to_onp::top_is(token_type_t token_type) const
            {
                return m_stack.empty() ? false : m_stack.top().get_type() == token_type;
            }
        }
    }
}
