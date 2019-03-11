#pragma once

#include "ast/type_name_reference.hpp"

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
        enum class builtin_function_kind;

        // Todo: rename to type_manipulator or so
        class type_builder
        {
        public:
            explicit type_builder(sema_type_factory& type_factory,
                                  sema_function_factory& function_factory,
                                  sema_context_factory& context_factory,
                                  sema_context_interface& parent_ctx,
                                  ast::type_name_reference name);

            type_builder& with_member(const member_info& member);
            type_builder& with_user_function(const sema_type& return_type, function_signature s);
            type_builder& with_builtin_function(const sema_type& return_type, function_signature s, builtin_function_kind kind);

            const sema_type& build_and_register_in_context();

            const sema_context_interface& context();

        private:
            sema_type_factory& m_type_factory;
            sema_function_factory& m_function_factory;
            sema_context_factory& m_context_factory;
            sema_context_interface& m_current_ctx;
            sema_context_interface& m_type_ctx;
            ast::type_name_reference m_name;
            std::vector<member_info> m_members;
        };
    }
}
