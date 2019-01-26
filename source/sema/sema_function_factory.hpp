#pragma once

#include "sema/sema_function.hpp"

#include <vector>

namespace cmsl
{
    namespace sema
    {
        class sema_function_impl;

        class sema_function_factory
        {
        public:
            sema_function_impl& create(const sema_context_interface& ctx, const sema_type& return_type, sema_function::function_signature s);

        private:
            std::vector<std::unique_ptr<sema_function>> m_functions;
        };
    }
}
