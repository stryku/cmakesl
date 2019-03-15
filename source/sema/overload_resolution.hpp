#pragma once

#include "lexer/token/token.hpp"
#include "sema/function_lookup_result.hpp"

#include <memory>

namespace cmsl
{
    namespace errors
    {
        class errors_observer;
    }

    namespace sema
    {
        class expression_node;

        class overload_resolution
        {
        public:
            // Todo: consider accepting source range instead of token
            explicit overload_resolution(errors::errors_observer& errs, lexer::token::token call_token);

            const sema_function* choose(const function_lookup_result_t& functions, const std::vector<std::unique_ptr<expression_node>>& call_parameters) const;
            const sema_function* choose(const single_scope_function_lookup_result_t& functions, const std::vector<std::unique_ptr<expression_node>>& call_parameters) const;
            const sema_function* choose(const single_scope_function_lookup_result_t& functions, const expression_node& call_parameter) const;

        private:
            const sema_function* choose_from_scope(const single_scope_function_lookup_result_t& functions,
                                                   const std::vector<std::unique_ptr<expression_node>>& call_parameters) const;

            bool params_match(const sema_function& function, const std::vector<std::unique_ptr<expression_node>>& call_parameters) const;

            void raise_wrong_call_error(const single_scope_function_lookup_result_t& functions, const std::vector<std::unique_ptr<expression_node>>& call_parameters) const;
            void raise_wrong_call_error(const single_scope_function_lookup_result_t& functions, const expression_node& call_parameter) const;

        private:
            errors::errors_observer& m_errs;
            lexer::token::token m_call_token;
        };
    }
}
