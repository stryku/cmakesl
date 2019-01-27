#pragma once

#include "lexer/token/token.hpp"
#include "sema/function_signature.hpp"

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
            virtual ~sema_function() = default;
            virtual const block_node& body() const = 0;
            virtual const function_signature& signature() const = 0;
            virtual const sema_type& return_type() const = 0;
        };
    }
}