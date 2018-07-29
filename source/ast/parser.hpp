#pragma once

#include "ast/parameter_declaration.hpp"
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
        class type;

        class parser
        {
        public:
            using token_it = lexer::token::token_container_t::const_iterator;
            using token_type_t = lexer::token::token_type;

            parser(errors::errors_observer& err_observer, const lexer::token::token_container_t& tokens);

            boost::optional<type> get_type();
            std::vector<parameter_declaration> get_parameters_declaration();

        private:
            bool eat(boost::optional<token_type_t> type = {});
            bool eat_type();

            bool expect_not_at_end();
            bool expect_token(token_type_t type);
            bool expect_token_other_than(token_type_t type);

            bool is_builtin_type(token_type_t token_type) const;
            bool current_is(token_type_t token_type) const;

            void raise_error();

            boost::optional<parameter_declaration> get_parameter_declaration();
            // Returns true on success, false otherwise
            bool prepare_for_next_parameter_declaration();

            bool is_at_end() const;

        private:
            errors::errors_observer& m_err_observer;
            token_it m_token;
            token_it m_end;
        };
    }
}
