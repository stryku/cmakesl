#pragma once

#include "lexer/token/token.hpp"
#include "sema/function_signature.hpp"

#include <memory>
#include <vector>

namespace cmsl
{
    namespace sema
    {
        class sema_context_interface;
        class sema_context;
        class user_sema_function;
        enum class builtin_function_kind;
        class builtin_sema_function;
        class sema_function;
        class sema_type;
        struct member_info;

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
            user_sema_function& create_user(const sema_context_interface& ctx, const sema_type& return_type, function_signature s);
            builtin_sema_function& create_builtin(const sema_context_interface& ctx, const sema_type& return_type, function_signature s, builtin_function_kind kind);
        };

        class sema_type_factory : public factory<sema_type>
        {
        public:
            const sema_type& create(const sema_context_interface& ctx, lexer::token::token name, std::vector<member_info> members);
        };

        class sema_context_factory : public factory<sema_context_interface>
        {
        public:
            sema_context& create(const sema_context_interface* parent);
            sema_context& create_class(const sema_context_interface* parent);
        };
    }
}

