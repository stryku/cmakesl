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

        const lexer::token::token_container_t& infix_node::get_tokens() const
        {
            return m_tokens;
        }
    }
}
