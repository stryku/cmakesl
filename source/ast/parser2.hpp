#pragma once

#include "ast/parameter_declaration.hpp"
#include "ast/parser_utils.hpp"
#include "ast/parse_errors_observer.hpp"

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
        class conditional_node;
        class block_node;
        class type_representation;

        // Todo: rename methods to parse_*
        class parser2 : public parser_utils
        {
        public:
            parser2(errors::errors_observer& err_observer, cmsl::source_view source, const token_container_t& tokens);

            std::unique_ptr<ast_node> translation_unit();
            std::unique_ptr<ast_node> variable_declaration();
            std::unique_ptr<ast_node> function();
            std::unique_ptr<ast_node> class_();
            std::unique_ptr<ast_node> factor();

            // expr() doesn't eat terminating semicolon. Callee has to handle that if there is a need.
            std::unique_ptr<ast_node> expr();
            std::unique_ptr<ast_node> get_if_else_node();
            std::unique_ptr<ast_node> get_while_node();
            std::unique_ptr<ast_node> get_return_node();
            boost::optional<type_representation> type();
            std::unique_ptr<block_node> block();

        private:
            struct function_call_values
            {
                token_t name;
                std::vector<std::unique_ptr<ast_node>> params;
            };

            boost::optional<token_t> eat_function_call_name();

            bool current_is_class_member_access() const;
            bool current_is_function_call() const;
            bool current_is_fundamental_value() const;
            bool function_declaration_starts() const;
            bool declaration_starts() const;

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

            std::unique_ptr<ast_node> fundamental_value();
            std::unique_ptr<ast_node> function_call();

            boost::optional<std::vector<std::unique_ptr<ast_node>>> parameter_list();
            boost::optional<function_call_values> get_function_call_values();

            bool prepare_for_next_parameter_declaration();
            boost::optional<param_declaration> get_param_declaration();
            boost::optional<std::vector<param_declaration>> param_declarations();

            std::unique_ptr<conditional_node> get_conditional_node();

            std::unique_ptr<ast_node> constructor(token_t class_name);

        private:
            parse_errors_reporter m_errors_reporter;
        };
    }
}
