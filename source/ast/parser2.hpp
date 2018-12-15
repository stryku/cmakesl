#pragma once

#include "ast/type_reference.hpp"
#include "lexer/token/token.hpp"

#include <boost/optional.hpp>

#include <memory>

namespace cmsl
{
    namespace errors
    {
        class errors_observer;
    }

    namespace ast
    {
        class ast_node;

        class parser2
        {
        private:
            using token_container_t = lexer::token::token_container_t;
            using token_it = token_container_t::const_iterator;
            using token_type_t = lexer::token::token_type;
            using token_t = lexer::token::token;

        public:
            parser2(errors::errors_observer& err_observer, const token_container_t& tokens);

            std::unique_ptr<ast_node> translation_unit();
            std::unique_ptr<ast_node> variable_declaration();
            std::unique_ptr<ast_node> infix();


        private:
            void raise_error();

            bool expect_not_at_end();
            bool is_at_end() const;

            token_type_t peek(size_t n = 1u) const;
            boost::optional<token_t> eat(boost::optional<token_type_t> type = {});
            boost::optional<token_t> eat_generic_type();
            boost::optional<token_t> eat_simple_type();


            boost::optional<type_reference> type();
            boost::optional<type_reference> generic_type();
            boost::optional<type_reference> simple_type();
            bool generic_type_starts() const;

            bool next_is(token_type_t token_type) const;
            bool current_is_generic_type() const;
            bool current_is(token_type_t token_type) const;
            bool is_builtin_simple_type(token_type_t token_type) const;


        private:
            errors::errors_observer& m_err_observer;
            token_it m_token;
            token_it m_end;
        };
    }
}
