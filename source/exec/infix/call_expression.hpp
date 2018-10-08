#pragma once

#include "exec/infix/infix_expression.hpp"

#include "lexer/token/token.hpp"

namespace cmsl
{
    namespace ast
    {
        class function;
    }

    namespace exec
    {
        class function_caller;

        namespace infix
        {
            class call_expression : public infix_expression
            {
            public:
                using params_t = std::vector<std::unique_ptr<infix_expression>>;

                explicit call_expression(function_caller& fun_caller, lexer::token::token name, params_t parameter_expressions);

                lexer::token::token get_name() const;
                std::vector<const infix_expression*> get_param_expressions() const;
                
            protected:
                function_caller& get_caller() const;
                cmsl::string_view get_name_view() const;
                std::vector<inst::instance*> evaluate_params(infix_evaluation_context& ctx) const;

                const ast::function& get_function(infix_evaluation_context& ctx) const;

            private:
                function_caller& m_fun_caller;
                lexer::token::token m_name;
                params_t m_parameter_expressions;
            };
        }
    }
}
