#include "exec/stmt/infix_to_onp.hpp"

#include "ast/ast_context.hpp"
#include "lexer/token/token.hpp"

#include <stack>

namespace cmsl
{
    namespace exec
    {
        namespace stmt
        {
            infix_to_onp::infix_to_onp(const tokens_container_t& infix_tokens, const ast::ast_context& ctx)
                : m_token{ std::cbegin(infix_tokens) }
                , m_end{ std::cend(infix_tokens) }
                , m_ast_ctx{ ctx }
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
                    handle_token(*m_token);
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
                    const auto fun = m_ast_ctx.find_function(token.str());
                    if (!fun)
                    {
                        m_out.emplace_back(token);
                    }
                    else
                    {
                        convert_function_call();
                    }
                }
                else if (token_type == token_type_t::plus)
                {
                    while (!m_stack.empty())
                    {
                        const auto op = m_stack.top();
                        m_stack.pop();
                        m_out.emplace_back(op);
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
        }
    }
}
