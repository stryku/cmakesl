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
            std::unique_ptr<ast_node> initializer_list();

            // expr() doesn't eat terminating semicolon. Callee has to handle that if there is a need.
            std::unique_ptr<ast_node> expr();
            std::unique_ptr<ast_node> get_if_else_node();
            std::unique_ptr<ast_node> get_while_node();
            std::unique_ptr<ast_node> get_return_node();
            std::optional<type_representation> type();
            std::unique_ptr<block_node> block();

        private:
            struct function_call_values
            {
                token_t name;
                token_t open_paren;
                std::vector<std::unique_ptr<ast_node>> params;
                token_t close_paren;
            };

            std::optional<token_t> eat_function_call_name();

            bool current_is_class_member_access() const;
            bool current_is_function_call() const;
            bool current_is_fundamental_value() const;
            bool function_declaration_starts() const;
            bool declaration_starts() const;

            static constexpr auto k_min_precedence{ 2u };
            static constexpr auto k_max_precedence{ 16u };
            std::unique_ptr<ast_node> parse_operator(unsigned precedence = k_max_precedence);

            std::unique_ptr<ast_node> fundamental_value();
            std::unique_ptr<ast_node> function_call();

            std::optional<std::vector<std::unique_ptr<ast_node>>> comma_separated_expression_list(token_type_t valid_end_of_list_token);

            struct call_param_list_values
            {
                token_t open_paren;
                std::vector<std::unique_ptr<ast_node>> params;
                token_t close_paren;
            };
            std::optional<call_param_list_values> parameter_list();
            std::optional<function_call_values> get_function_call_values();

            bool prepare_for_next_parameter_declaration();
            std::optional<param_declaration> get_param_declaration();

            struct param_list_values
            {
                token_t open_paren;
                std::vector<param_declaration> params;
                token_t close_paren;
            };
            std::optional<param_list_values> param_declarations();

            std::unique_ptr<conditional_node> get_conditional_node();

            std::unique_ptr<ast_node> constructor(token_t class_name);

        private:
            parse_errors_reporter m_errors_reporter;
        };
    }
}
