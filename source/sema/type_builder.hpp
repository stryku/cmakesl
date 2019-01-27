#pragma once

#include "sema/function_signature.hpp"

namespace cmsl
{
    namespace sema
    {
        struct member_info;
        class sema_context;
        class sema_context_interface;
        class sema_type_factory;
        class sema_function_factory;
        class sema_context_factory;

        class type_builder
        {
        public:
            explicit type_builder(sema_type_factory& type_factory,
                                  sema_function_factory& function_factory,
                                  sema_context_factory& context_factory,
                                  sema_context& parent_ast_ctx,
                                  lexer::token::token name);

            type_builder& with_member(const member_info& member);
            type_builder& with_function(const sema_type& return_type, function_signature s);

            const sema_type& build();

            const sema_context_interface& context();

        private:
            sema_type_factory& m_type_factory;
            sema_function_factory& m_function_factory;
            sema_context_factory& m_context_factory;
            sema_context& m_ctx;
            lexer::token::token m_name;
            std::vector<member_info> m_members;
        };
    }
}
