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
            using token_type_t = lexer::token::token_type;

            void add_types();
            void add_functions();

            void add_bool_type();

            template <unsigned N>
            lexer::token::token make_token(token_type_t token_type, const char(&tok)[N]);

        private:
            sema_type_factory &m_type_factory;
            sema_function_factory &m_function_factory;
            sema_context_factory &m_context_factory;
        };
    }
}