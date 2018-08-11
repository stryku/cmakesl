#include "ast/infix_expression.hpp"

#include <stack>

namespace cmsl
{
    namespace ast
    {
        infix_expression::infix_expression(lexer::token::token_container_t tokens)
            : ast_node{ expression_type::infix }
            , m_tokens{ std::move(tokens) }
        {}

        int infix_expression::evaluate()
        {
            std::stack<int> stack;

            for (const auto& token : m_tokens)
            {
                if (token.get_type() == lexer::token::token_type::integer)
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
                        case lexer::token::token_type::plus:
                            stack.push(b + a);
                            break;
                        case lexer::token::token_type::minus:
                            stack.push(b - a);
                            break;
                    }
                }
            }

            return stack.top();
        }
    }
}
