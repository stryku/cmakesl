#pragma once

#include "common/string.hpp"
#include "common/source_view.hpp"
#include "token.hpp"
#include "lexer/source_location_manipulator.hpp"

#include <string>
#include <vector>
#include <unordered_map>

namespace cmsl
{
    namespace errors
    {
        class errors_observer;
    }

    namespace lexer
    {
        class lexer
        {
        private:
            using source_t = cmsl::source_view;
            using source_it_t = cmsl::string_view::const_iterator;

        public:
            lexer(errors::errors_observer& err_observer, source_t source);

            std::vector<token> lex();

        private:
            struct arithmetical_token_definition
            {
                token_type single;
                token_type op_equal;
                token_type twice;

                bool has_twice() const
                {
                    return twice != token_type::undef;
                }
            };

            using aritmetical_token_definition_t = std::unordered_map<char, arithmetical_token_definition>;
            aritmetical_token_definition_t create_arithmetical_token_definitions() const;

            using one_char_tokens_t = std::unordered_map<char, token_type>;
            one_char_tokens_t create_one_char_tokens() const;

            using keyword_tokens_t = string_view_map<token_type>;
            keyword_tokens_t create_keyword_tokens() const;

            token get_next_token();
            token_type get_next_token_type();
            token_type get_numeric_token_type();
            token_type get_identifier_or_keyword_token_type();
            token_type get_equal_token_type();
            token_type get_string_token_type();
            token_type get_arithmetical_token_type(char operator_char);
            token_type get_one_char_token_type(char c);

            void consume_integer();
            void consume_char();
            void consume_whitespaces();

            bool is_end() const;
            bool has_next() const;

            bool is_identifier_char(char c) const;
            bool is_arithmetical_operator(char c) const;
            bool is_one_char_token(char c) const;
            bool is_whitespace(char c) const;

            char current() const;
            char next() const;

        private:
            errors::errors_observer& m_err_observer;
            const source_t m_source;
            source_location_manipulator m_source_loc;
            const aritmetical_token_definition_t m_aritmetical_token_definitions;
            const one_char_tokens_t m_one_char_tokens;
            const keyword_tokens_t m_keyword_tokens;
        };
    }
}
