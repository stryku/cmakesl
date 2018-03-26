#include "lexer/lexer.hpp"

#include <boost/assert.hpp>

#include <cctype>

namespace cmsl
{
    namespace lexer
    {
        lexer::lexer(source_t source)
            : m_source{ source }
            , m_current_pos{ m_source.begin() }
            , m_aritmetical_token_definitions{ create_arithmetical_token_definitions() }
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
            if (curr == '.')
            {
                if (is_next() && std::isdigit(next())) // .123
                {
                    return get_numeric_token();
                }
                else
                {
                    consume_char();
                    return token_t{ token_t::token_type_t::dot };
                }
            }
            if (curr == '{')
            {
                consume_char();
                return token_t{ token_t::token_type_t::open_brace };
            }
            if (curr == '}')
            {
                consume_char();
                return token_t{ token_t::token_type_t::close_brace };
            }
            if (curr == '[')
            {
                consume_char();
                return token_t{ token_t::token_type_t::open_square };
            }
            if (curr == ']')
            {
                consume_char();
                return token_t{ token_t::token_type_t::close_square };
            }
            if (curr == '(')
            {
                consume_char();
                return token_t{ token_t::token_type_t::open_paren };
            }
            if (curr == ')')
            {
                consume_char();
                return token_t{ token_t::token_type_t::close_paren };
            }
            if (curr == '=')
            {
                return get_equal_token();
            }
            if (is_arithmetical_operator(curr))
            {
                return get_arithmetical_token(curr);
            }
            if (std::isalpha(curr) || curr == '_')
            {
                return get_identifier_token();
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
                consume_char();
            }
        }

        void lexer::consume_char()
        {
            assert(!is_end());
            ++m_current_pos;
        }

        lexer::token_t lexer::get_identifier_token()
        {
            while (is_identifier_char(*m_current_pos))
            {
                consume_char();
            }

            return token_t{ token_t::token_type_t::identifier };
        }

        bool lexer::is_identifier_char(char c) const
        {
            // At this point we know that identifier won't start with a digit, so we can use isalnum()
            return std::isalnum(c) || c == '_';
        }

        lexer::token_t lexer::get_equal_token()
        {
            // current == '='
            ++m_current_pos;

            if (current() == '=') // ==
            {
                consume_char();
                return token_t{ token_t::token_type_t::equalequal };
            }

            // =
            return token_t{ token_t::token_type_t::equal };
        }

        lexer::token_t lexer::get_minus_token()
        {
            // current == '-'
            ++m_current_pos;

            if (current() == '-') // --
            {
                consume_char();
                return token_t{ token_t::token_type_t::equalequal };
            }

            // =
            return token_t{ token_t::token_type_t::equal };
        }

        lexer::token_t lexer::get_arithmetical_token(char operator_char)
        {
            // current() == operator_char, go to next char
            consume_char();

            const auto found = m_aritmetical_token_definitions.find(operator_char);
            assert(found != m_aritmetical_token_definitions.end());

            const auto& definition = found->second;

            if (is_end())
            {
                return token_t{ definition.single };
            }

            if (definition.has_twice() && current() == operator_char) // same, e.g. ++
            {
                consume_char();
                return token_t{ definition.twice };
            }

            if (current() == '=') // e.g. +=
            {
                consume_char();
                return token_t{ definition.op_equal };
            }

            return token_t{ definition.single };
        }

        lexer::aritmetical_token_definition_t lexer::create_arithmetical_token_definitions() const
        {
            using def_t = aritmetical_token_definition;

            aritmetical_token_definition_t definitions;

            definitions['-'] = def_t{ token_t::token_type_t::minus, token_t::token_type_t::minusequal, token_t::token_type_t::minusminus };
            definitions['+'] = def_t{ token_t::token_type_t::plus, token_t::token_type_t::plusequal, token_t::token_type_t::plusplus };
            definitions['&'] = def_t{ token_t::token_type_t::amp, token_t::token_type_t::ampequal, token_t::token_type_t::ampamp };
            definitions['|'] = def_t{ token_t::token_type_t::pipe, token_t::token_type_t::pipeequal, token_t::token_type_t::pipepipe };
            definitions['/'] = def_t{ token_t::token_type_t::slash, token_t::token_type_t::slashequal };
            definitions['*'] = def_t{ token_t::token_type_t::star, token_t::token_type_t::starequal };
            definitions['%'] = def_t{ token_t::token_type_t::percent, token_t::token_type_t::percentequal };
            definitions['!'] = def_t{ token_t::token_type_t::exclaim, token_t::token_type_t::exclaimequal };
            definitions['^'] = def_t{ token_t::token_type_t::xor, token_t::token_type_t::xorequal };

            return definitions;
        }

        bool lexer::is_arithmetical_operator(char c) const
        {
            const auto arith_operators = cmsl::string_view{ "-+&|/*%!^" };
            return arith_operators.find(c) != cmsl::string_view::npos;
        }
    }
}
