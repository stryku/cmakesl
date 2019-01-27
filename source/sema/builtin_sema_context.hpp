#pragma once

#include "sema/sema_context.hpp"

namespace cmsl
{
    namespace sema
    {
        class sema_function_factory;
        class sema_type_factory;
        class sema_context_factory;

        class builtin_sema_context : public sema_context
        {
        public:
            explicit builtin_sema_context(sema_type_factory &type_factory,
                                          sema_function_factory &function_factory,
                                          sema_context_factory &context_factory);

        private:
            void add_types();
            void add_functions();

            void add_bool_type();

        private:
            sema_type_factory &m_type_factory;
            sema_function_factory &m_function_factory;
            sema_context_factory &m_context_factory;
        };
    }
}