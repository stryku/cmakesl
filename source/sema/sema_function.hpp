#pragma once

#include "lexer/token/token.hpp"

namespace cmsl
{
    namespace sema
    {
        class sema_context_interface;
        class sema_type;
        class block_node;

        class sema_function
        {
        public:
            struct parameter_declaration
            {
                const sema_type& ty;
                lexer::token::token name;
            };

            struct function_signature
            {
                lexer::token::token name;
                std::vector<parameter_declaration> params;
            };

            virtual ~sema_function() = default;
            virtual const block_node& body() const = 0;
            virtual const function_signature& signature() const = 0;
            virtual const sema_type& return_type() const = 0;
        };
    }
}