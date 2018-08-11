#include "errors/errors_observer.hpp"
#include "errors/error.hpp"
#include "lexer/lexer.hpp"

#include <boost/assert.hpp>

#include <cctype>

namespace cmsl
{
    namespace lexer
    {
        lexer::lexer(errors::errors_observer& err_observer, source_t source)
            : m_err_observer{ err_observer }
            , m_source{ source }
            , m_source_loc{ m_source }
            , m_aritmetical_token_definitions{ create_arithmetical_token_definitions() }
            , m_one_char_tokens{ create_one_char_tokens() }
        {}

        std::vector<token::token> lexer::lex()
        {
            auto tokens = std::vector<token::token>{};
            const auto end = m_source.cend();

            while (!is_end())
            {
                const auto t = get_next_token();

                if (t.get_type() != token_t::token_type_t::undef)
                {
                    tokens.push_back(t);
                }
            }

            return tokens;
        }

        bool lexer::is_end() const
        {
            return m_source_loc.is_at_end();
        }

        bool lexer::has_next() const
        {
            return m_source_loc.has_next();
        }

        char lexer::current() const
        {
            return m_source_loc.current_char();
        }

        char lexer::next() const
        {
            return m_source_loc.next_char();
        }

        lexer::token_t lexer::get_next_token()
        {
            consume_whitespaces();
            const auto begin_loc = m_source_loc.location();
            const auto token_type = get_next_token_type();
            const auto end_loc = m_source_loc.location();
            return token_t{ token_type, source_range{ begin_loc, end_loc }, m_source };
        }

        lexer::token_t::token_type_t lexer::get_next_token_type()
        {

            if (is_end())
            {
                // in such case, there are whitespaces at the end of source. No other chars
                return token_t::token_type_t::undef;
            }

            const auto curr = current();

            if (std::isdigit(curr))
            {
                return get_numeric_token_type();
            }
            if (curr == '.')
            {
                if (has_next() && std::isdigit(next())) // .123
                {
                    return get_numeric_token_type();
                }
                else
                {
                    consume_char();
                    return token_t::token_type_t::dot;
                }
            }
            if (curr == '=')
            {
                return get_equal_token_type();
            }
            if (curr == '"')
            {
                return get_string_token_type();
            }
            if (is_one_char_token(curr))
            {
                return get_one_char_token_type(curr);
            }
            if (is_arithmetical_operator(curr))
            {
                return get_arithmetical_token_type(curr);
            }
            if (std::isalpha(curr) || curr == '_')
            {
                return get_identifier_or_keyword_token_type();
            }

            return token_t::token_type_t::undef;
        }

        lexer::token_t::token_type_t lexer::get_numeric_token_type()
        {
            consume_integer();

            if (is_end())
            {
                return token_t::token_type_t::integer;
            }

            // Handle real e.g. 123.
            if (current() == '.')
            {
                m_source_loc.consume_char();
                consume_integer();
                return token_t::token_type_t::real;
            }

            // TODO: handle hex, oct and bin

            return token_t::token_type_t::integer;
        }

        void lexer::consume_integer()
        {
            while (!is_end() && std::isdigit(current()))
            {
                consume_char();
            }
        }

        void lexer::consume_char()
        {
            assert(!is_end());
            m_source_loc.consume_char();
        }

        void lexer::consume_whitespaces()
        {
            assert(!is_end());
            while (!is_end() && is_whitespace(current()))
            {
                consume_char();
            }
        }


        lexer::token_t::token_type_t lexer::get_identifier_or_keyword_token_type()
        {
            std::string token_val;
            while (is_identifier_char(current()))
            {
                token_val += current();
                consume_char();
            }

            if (token_val == "int")
            {
                return token_t::token_type_t::t_int;
            }
            else if (token_val == "real")
            {
                return token_t::token_type_t::t_real;
            }
            else if (token_val == "return")
            {
                return token_t::token_type_t::return_keyword;
            }

            return token_t::token_type_t::identifier;
        }

        bool lexer::is_identifier_char(char c) const
        {
            // At this point we know that identifier won't start with a digit, so we can use isalnum()
            return std::isalnum(c) || c == '_';
        }

        lexer::token_t::token_type_t lexer::get_equal_token_type()
        {
            // current == '='
            m_source_loc.consume_char();

            if (current() == '=') // ==
            {
                consume_char();
                return token_t::token_type_t::equalequal;
            }

            // =
            return token_t::token_type_t::equal;
        }

        lexer::token_t::token_type_t lexer::get_string_token_type()
        {
            consume_char();

            // TODO: handle new lines
            while (!is_end() && current() != '"')
            {
                if (current() == '\\')
                {
                    // Doesn't matter what is after '\', we need to consume it
                    consume_char();

                    if (is_end())
                    {
                        break;
                    }
                }

                consume_char();
            }

            if (is_end())
            {
                // TODO: pass proper error
                m_err_observer.nofify_error(errors::error{});
                return token_t::token_type_t::undef;
            }

            // current() == '"', consume
            consume_char();

            return token_t::token_type_t::string;
        }

        lexer::token_t::token_type_t lexer::get_arithmetical_token_type(char operator_char)
        {
            // current() == operator_char, go to next char
            consume_char();

            const auto found = m_aritmetical_token_definitions.find(operator_char);
            assert(found != m_aritmetical_token_definitions.end());

            const auto& definition = found->second;

            if (is_end())
            {
                return definition.single;
            }

            if (definition.has_twice() && current() == operator_char) // same, e.g. ++
            {
                consume_char();
                return definition.twice;
            }

            if (current() == '=') // e.g. +=
            {
                consume_char();
                return definition.op_equal;
            }

            return definition.single;
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

        lexer::one_char_tokens_t lexer::create_one_char_tokens() const
        {
            one_char_tokens_t tokens;

            tokens['('] = token_t::token_type_t::open_paren;
            tokens[')'] = token_t::token_type_t::close_paren;
            tokens['{'] = token_t::token_type_t::open_brace;
            tokens['}'] = token_t::token_type_t::close_brace;
            tokens['['] = token_t::token_type_t::open_square;
            tokens[']'] = token_t::token_type_t::close_square;
            tokens[';'] = token_t::token_type_t::semicolon;

            return tokens;
        }

        bool lexer::is_arithmetical_operator(char c) const
        {
            const auto arith_operators = cmsl::string_view{ "-+&|/*%!^" };
            return arith_operators.find(c) != cmsl::string_view::npos;
        }

        bool lexer::is_one_char_token(char c) const
        {
            const auto chars = cmsl::string_view{ "{}[]();" };
            return chars.find(c) != cmsl::string_view::npos;
        }

        bool lexer::is_whitespace(char c) const
        {
            const auto chars = cmsl::string_view{ " \t\n\r" };
            return chars.find(c) != cmsl::string_view::npos;
        }

        lexer::token_t::token_type_t lexer::get_one_char_token_type(char c)
        {
            const auto found = m_one_char_tokens.find(c);
            assert(found != m_one_char_tokens.end());

            consume_char();

            const auto token_type = found->second;
            return token_type;
        }
    }
}
