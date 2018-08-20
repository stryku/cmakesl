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

            void infix_to_onp::convert_function_call()
            {
                const auto fun = *m_token++;
                m_stack.push(fun);

                if (!eat(token_type_t::open_paren))
                {
                    // todo raise error
                }

                while (m_token != m_end && m_token->get_type() != token_type_t::close_paren)
                {
                    handle_token(*m_token++);
                }

                if (!eat(token_type_t::close_paren))
                {
                    // todo raise error
                }
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
                    token_type_t::plus,
                    token_type_t::minus,
                    token_type_t::equal
                    // todo add rest of operators
                };

                return contains(operators, token_type);
            }

            infix_to_onp::operator_precedences_t infix_to_onp::get_operator_precedences() const
            {
                operator_precedences_t prec;

                // From https://en.cppreference.com/w/cpp/language/operator_precedence
                prec[token_type_t::dot] = 2;
                prec[token_type_t::plus] = 6;
                prec[token_type_t::minus] = 6;
                prec[token_type_t::equal] = 16;
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
        }
    }
}
