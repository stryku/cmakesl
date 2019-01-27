#pragma once

#include "sema/sema_context.hpp"
#include "function_signature.hpp"
#include "builtin_function_kind.hpp"

namespace cmsl
{
    namespace exec
    {
        namespace inst
        {
            class instance;
        }
    }

    namespace sema
    {
        class sema_function_factory;
        class sema_type_factory;
        class sema_context_factory;
        enum class builtin_function_kind;
        class type_builder;

        class builtin_sema_context : public sema_context
        {
        public:
            explicit builtin_sema_context(sema_type_factory &type_factory,
                                          sema_function_factory &function_factory,
                                          sema_context_factory &context_factory);

        private:
            using token_type_t = lexer::token::token_type;
            struct builtin_function_info
            {
                const sema_type& return_type;
                function_signature signature;
                builtin_function_kind kind;
            };

            // Types are added in two phases:
            // 1. Adding types with its members to sema context.
            // 2. Adding member functions to already created members.
            // It's done this way because member function can refer to type that has not been created yet.
            // E.g. bool has method `string to_string()` which returns `string`,
            // and string has method `bool empty()` which returns `bool`
            void add_types();
            void add_functions();

            template <typename Functions>
            void add_type_member_functions(type_builder& manipulator, Functions&& functions);

            type_builder add_bool_type();
            void add_bool_member_functions(type_builder& bool_manipulator);
            type_builder add_int_type();
            void add_int_member_functions(type_builder& int_manipulator);
            type_builder add_double_type();
            void add_double_member_functions(type_builder& double_manipulator);

            template <unsigned N>
            lexer::token::token make_token(token_type_t token_type, const char(&tok)[N]);

            template <unsigned N>
            lexer::token::token make_id_token(const char(&tok)[N]);

        private:
            sema_type_factory &m_type_factory;
            sema_function_factory &m_function_factory;
            sema_context_factory &m_context_factory;
        };
    }
}