#pragma once

#include "lexer/token.hpp"
#include "sema/function_signature.hpp"

#include <memory>
#include <vector>
#include <ast/type_representation.hpp>

namespace cmsl::sema
{
        class sema_context_interface;
        class sema_context;
        class user_sema_function;
        enum class builtin_function_kind;
        class builtin_sema_function;
        class sema_function;
        class sema_type;
        struct member_info;
        class type_builder;

        template <typename StoredType>
        class factory
        {
        protected:
            template <typename TypeImplementation, typename... Args>
            TypeImplementation& create_impl(Args&&... args)
            {
                auto val= std::make_unique<TypeImplementation>(std::forward<Args>(args)...);
                auto ptr = val.get();
                m_storage.emplace_back(std::move(val));
                return *ptr;
            }

        private:
            std::vector<std::unique_ptr<StoredType>> m_storage;
        };

        class sema_function_factory : public factory<sema_function>
        {
        public:
            ~sema_function_factory();

            user_sema_function& create_user(const sema_context_interface& ctx, const sema_type& return_type, function_signature s);
            builtin_sema_function& create_builtin(const sema_context_interface& ctx, const sema_type& return_type, function_signature s, builtin_function_kind kind);
        };

        class sema_context_factory : public factory<sema_context_interface>
        {
        public:
            ~sema_context_factory();
            sema_context& create(const sema_context_interface* parent);
            sema_context& create_class(const sema_context_interface* parent);
        };

        class sema_type_factory : public factory<sema_type>
        {
        public:
            ~sema_type_factory();
            const sema_type& create(const sema_context_interface& ctx, ast::type_representation name, std::vector<member_info> members);
            const sema_type& create_reference(const sema_type& referenced_type);
            const sema_type& create_homogeneous_generic(const sema_context_interface &ctx,
                                                        ast::type_representation name,
                                                        const sema_type &value_type);
        };

        class sema_generic_type_factory : public sema_type_factory
        {
        public:
            explicit sema_generic_type_factory(sema_context_interface& generic_types_context,
                                               const sema_context_interface& creation_context,
                                               sema_type_factory& type_factory,
                                               sema_function_factory& function_factory,
                                               sema_context_factory& context_factory);

            const sema_type* create_generic(const ast::type_representation& name);

        private:
            const sema_type* try_get_or_create_value_type(const ast::type_representation& name);
            const sema_type* create_list(const ast::type_representation& name);

        private:
            sema_context_interface& m_generic_types_context;
            const sema_context_interface& m_creation_context;
            sema_type_factory& m_type_factory;
            sema_function_factory& m_function_factory;
            sema_context_factory& m_context_factory;
        };
}

