#pragma once

#include "common/algorithm.hpp"
#include "lexer/token/token.hpp"

#include <memory>
#include <vector>

namespace cmsl
{
    namespace exec
    {
        class function_caller;

        namespace infix
        {
            class infix_expression;

            class infix_expression_builder
            {
            private:
                using token_t = lexer::token::token;
                using token_type_t = lexer::token::token_type;
                using tokens_t = lexer::token::token_container_t;
                using token_it_t = tokens_t::const_iterator;

            public:
                explicit infix_expression_builder(function_caller& fun_caller, const tokens_t &tokens);

                std::unique_ptr<infix_expression> build();

            private:
                void eat();
                bool expect_not_at_end() const;
                bool expect_token(token_type_t expected_type) const;
                bool eat(token_type_t expected_type);
                bool is_current_fundamental_value() const;
                bool is_next(token_type_t expected_type) const;
                token_type_t peek(size_t n) const;

                bool is_current_function_call() const;

                template <typename Collection>
                bool current_is_one_of(Collection&& collection)
                {
                    return contains(std::forward<Collection>(collection), curr_type());
                }

                token_type_t curr_type() const;

                struct function_call_values
                {
                    token_t name;
                    std::vector<std::unique_ptr<infix_expression>> params;
                };

                function_call_values get_function_call_values();

                std::vector<std::unique_ptr<infix_expression>> parameter_list();
                std::unique_ptr<infix_expression> function_call();
                bool is_current_operator_5() const;
                std::unique_ptr<infix_expression> operator_expr_5();
                bool is_current_operator_6() const;
                std::unique_ptr<infix_expression> operator_expr_6();
                std::unique_ptr<infix_expression> expr();
                std::unique_ptr<infix_expression> factor();
                std::unique_ptr<infix_expression> fundamental_value();


            private:
                token_it_t m_token;
                token_it_t m_end;
                function_caller& m_fun_caller;
            };
        }
    }
}
