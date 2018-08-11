#include "ast/infix_node.hpp"

#include <stack>

namespace cmsl
{
    namespace ast
    {
        infix_node::infix_node(lexer::token::token_container_t tokens)
            : ast_node{ ast_node_type::infix }
            , m_tokens{ std::move(tokens) }
        {}

        int infix_node::evaluate()
        {
            std::stack<int> stack;

            for (const auto& token : to_onp())
            {
                if (token.get_type() == token_type_t::integer)
                {
                    const auto val = std::stoi(token.str().to_string());
                    stack.push(val);
                }
                else
                {
                    const auto a = stack.top();
                    stack.pop();
                    const auto b = stack.top();
                    stack.pop();

                    switch (token.get_type())
                    {
                        case token_type_t::plus:
                            stack.push(b + a);
                            break;
                        case token_type_t::minus:
                            stack.push(b - a);
                            break;
                    }
                }
            }

            return stack.top();
        }

        lexer::token::token_container_t infix_node::to_onp() const
        {
            lexer::token::token_container_t result;
            std::stack<lexer::token::token> stack;

            for (const auto& token : m_tokens)
            {
                if (token.get_type() == token_type_t::integer)
                {
                    result.emplace_back(token);
                }
                else if (token.get_type() == token_type_t::plus)
                {
                    while (!stack.empty())
                    {
                        const auto op = stack.top();
                        stack.pop();
                        result.emplace_back(op);
                    }

                    stack.push(token);
                }
                else if (token.get_type() == token_type_t::open_paren)
                {
                    stack.push(token);
                }
                else if (token.get_type() == token_type_t::close_paren)
                {
                    while (!stack.empty() && stack.top().get_type() != token_type_t::open_paren)
                    {
                        const auto op = stack.top();
                        stack.pop();
                        result.emplace_back(op);
                    }

                    stack.pop();
                }
            }

            while (!stack.empty())
            {
                const auto tok = stack.top();
                stack.pop();
                result.emplace_back(tok);
            }

            return result;
        }
    }
}
