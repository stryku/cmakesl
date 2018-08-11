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
        class ast_context;
        class ast_node;
        class type;
        class function;
        class block_expression;
        class return_expression;
        class infix_expression;

        class parser
        {
        public:
            using token_container_t = lexer::token::token_container_t;
            using token_it = token_container_t::const_iterator;
            using token_type_t = lexer::token::token_type;

            parser(errors::errors_observer& err_observer, const token_container_t& tokens);

            type* get_type(ast_context& ctx);
            boost::optional<std::vector<parameter_declaration>> get_parameters_declaration(ast_context& ctx);
            std::unique_ptr<infix_expression> get_infix_expression();
            std::unique_ptr<block_expression> get_block_expression();
            std::unique_ptr<function> get_function(ast_context& ctx);
            std::unique_ptr<return_expression> get_return_expression();

        private:
            bool eat(boost::optional<token_type_t> type = {});
            bool eat_type();

            bool expect_not_at_end();
            bool expect_token(token_type_t type);
            bool expect_token_other_than(token_type_t type);

            bool is_builtin_type(token_type_t token_type) const;
            bool current_is_infix_token() const;
            bool current_is(token_type_t token_type) const;

            void raise_error();

            boost::optional<parameter_declaration> get_parameter_declaration(ast_context& ctx);
            // Returns true on success, false otherwise
            bool prepare_for_next_parameter_declaration();

            bool is_at_end() const;

            boost::optional<lexer::token::token> get_identifier();

        private:
            errors::errors_observer& m_err_observer;
            token_it m_token;
            token_it m_end;
        };
    }
}
