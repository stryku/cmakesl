#pragma once

#include "lexer/token/token.hpp"

#include <boost/optional.hpp>

namespace cmsl
{
    namespace errors
    {
        class errors_observer;
    }

    namespace ast
    {
        class parser_utils
        {
        protected:
            using token_container_t = lexer::token::token_container_t;
            using token_it = token_container_t::const_iterator;
            using token_type_t = lexer::token::token_type;
            using token_t = lexer::token::token;

            explicit parser_utils(errors::errors_observer &err_observer, token_it current, token_it end);

            bool expect_not_at_end();
            bool is_at_end() const;

            token_type_t peek(size_t n = 1u) const;
            boost::optional<token_t> eat(boost::optional<token_type_t> type = {});

            const token_t &current() const;
            token_type_t curr_type() const;
            bool next_is(token_type_t token_type) const;
            bool current_is(token_type_t token_type) const;

            void raise_error(lexer::token::token token, std::string message);

        private:
            errors::errors_observer &m_err_observer;
            token_it m_current;
            token_it m_end;
        };
    }
}