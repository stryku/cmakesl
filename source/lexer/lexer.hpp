#pragma once

#include "common/string.hpp"

#include <string>
#include <vector>

namespace cmsl
{
    namespace lexer
    {
        namespace token
        {
            class token;
        }

        class lexer
        {
        private:
            using source_t = cmsl::string_view;
            using source_it_t = source_t::const_iterator;
            using token_t = token::token;

        public:
            using tokens_container_t = std::vector<token_t>;

            explicit lexer(source_t source);

            tokens_container_t lex();

        private:
            token_t get_next_token();
            token_t get_numeric_token();

            bool is_end() const;

        private:
            const source_t m_source;
            source_it_t m_current_pos;
        };
    }
}
