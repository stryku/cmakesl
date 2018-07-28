#pragma once

#include "ast/parameter_declaration.hpp"
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
        class ast_node;
        class type;

        class parser
        {
        public:
            using token_it = lexer::token::token_container_t::const_iterator;
            using token_type_t = lexer::token::token_type;

            parser(errors::errors_observer& err_observer, token_it token);

            type get_type();
            std::vector<parameter_declaration> get_parameters_declaration();

        private:
            void eat(boost::optional<lexer::token::token_type> type = {});
            void eat_type();
            bool is_builtin_type(token_type_t token_type) const;

            void raise_error();

        private:
            errors::errors_observer& m_err_observer;
            token_it m_token;
        };
    }
}
