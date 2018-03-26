#include "lexer/lexer.hpp"
#include "lexer/token/token.hpp"

#include <boost/assert.hpp>

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

        bool lexer::is_next() const
        {
            return std::next(m_current_pos) != m_source.end();
        }

        char lexer::current() const
        {
            return *m_current_pos;
        }

        char lexer::next() const
        {
            return *std::next(m_current_pos);
        }

        lexer::token_t lexer::get_next_token()
        {
            const auto curr = current();

            if (std::isdigit(curr))
            {
                return get_numeric_token();
            }
            if (curr == '.') // .123
            {
                if (is_next() && std::isdigit(next())) // .123
                {
                    return get_numeric_token();
                }
                else
                {
                    ++m_current_pos;
                    assert(false); // Don't handle dot yet
                }
            }

            return token_t{};
        }

        lexer::token_t lexer::get_numeric_token()
        {
            consume_integer();

            if (is_end())
            {
                return token_t{ token_t::token_type_t::integer };
            }

            // Handle real e.g. 123.
            if (*m_current_pos == '.')
            {
                ++m_current_pos;
                consume_integer();
                return token_t{ token_t::token_type_t::real };
            }

            // TODO: handle hex, oct and bin

            return token_t{ token_t::token_type_t::integer };
        }

        void lexer::consume_integer()
        {
            while (!is_end() && std::isdigit(*m_current_pos))
            {
                ++m_current_pos;
            }
        }
    }
}
