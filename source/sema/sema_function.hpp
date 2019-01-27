#pragma once

#include "lexer/token/token.hpp"
#include "sema/function_signature.hpp"

namespace cmsl
{
    namespace sema
    {
        class sema_function
        {
        public:
            virtual ~sema_function() = default;
            virtual const function_signature& signature() const = 0;
            virtual const sema_type& return_type() const = 0;
        };

    }
}