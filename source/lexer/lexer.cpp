#include "lexer/lexer.hpp"
#include "lexer/token/token.hpp"

#include <cctype>

namespace cmsl
{
    namespace lexer
    {
        lexer::lexer(source_t source)
            : m_source{ source }
            , m_current_pos{ m_source.begin() }
        {}

        lexer::tokens_container_t lexer::lex()
        {
            auto tokens = tokens_container_t{};
            const auto end = m_source.cend();

            while (!is_end())
            {
                const auto t = get_next_token();
                tokens.push_back(t);
            }

            return tokens;
        }

        bool lexer::is_end() const
        {
            return m_current_pos == m_source.end();
        }

        lexer::token_t lexer::get_next_token()
        {
            if (std::isdigit(*m_current_pos))
            {
                return get_numeric_token();
            }

            return token_t{};
        }

        lexer::token_t lexer::get_numeric_token()
        {
            while (!is_end() && std::isdigit(*m_current_pos))
            {
                ++m_current_pos;
            }

            return token_t{ token_t::token_type_t::integer };
        }
    }
}
