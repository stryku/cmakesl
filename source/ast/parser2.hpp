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


        private:
            struct function_call_values
            {
                token_t name;
                std::vector<std::unique_ptr<ast_node>> params;
            };

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

            token_type_t curr_type() const;
            bool next_is(token_type_t token_type) const;
            bool is_builtin_simple_type(token_type_t token_type) const;
            bool current_is_generic_type() const;
            bool current_is(token_type_t token_type) const;
            bool current_is_class_member_access() const;
            bool current_is_function_call() const;
            bool current_is_fundamental_value() const;

            // It is ugly, but it is better to keep it this way for grammar purpose. Consider refactor, though.
            bool is_current_operator_2() const;
            bool is_current_operator_3() const;
            bool is_current_operator_5() const;
            bool is_current_operator_6() const;
            bool is_current_operator_9() const;
            bool is_current_operator_10() const;
            bool is_current_operator_11() const;
            bool is_current_operator_12() const;
            bool is_current_operator_13() const;
            bool is_current_operator_14() const;
            bool is_current_operator_15() const;
            bool is_current_operator_16() const;
            std::unique_ptr<ast_node> operator_2();
            std::unique_ptr<ast_node> operator_3();
            std::unique_ptr<ast_node> operator_5();
            std::unique_ptr<ast_node> operator_6();
            std::unique_ptr<ast_node> operator_9();
            std::unique_ptr<ast_node> operator_10();
            std::unique_ptr<ast_node> operator_11();
            std::unique_ptr<ast_node> operator_12();
            std::unique_ptr<ast_node> operator_13();
            std::unique_ptr<ast_node> operator_14();
            std::unique_ptr<ast_node> operator_15();
            std::unique_ptr<ast_node> operator_16();

            std::unique_ptr<ast_node> factor();
            std::unique_ptr<ast_node> fundamental_value();
            std::unique_ptr<ast_node> expr();
            std::unique_ptr<ast_node> function_call();

            std::vector<std::unique_ptr<ast_node>> parameter_list();
            function_call_values get_function_call_values();

        private:
            errors::errors_observer& m_err_observer;
            token_it m_token;
            token_it m_end;
        };
    }
}
