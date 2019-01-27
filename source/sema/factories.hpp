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
        class sema_function_impl;
        class sema_function;
        class sema_type;
        struct member_info;

        template <typename Ty>
        class factory
        {
        protected:
            template <typename... Args>
            Ty& create_impl(Args&&... args)
            {
                auto val= std::make_unique<Ty>(std::forward<Args>(args)...);
                auto ptr = val.get();
                m_storage.emplace_back(std::move(m_storage));
                return *ptr;
            }

        private:
            std::vector<std::unique_ptr<sema_function>> m_storage;
        };

        class sema_function_factory : public factory<sema_function_impl>
        {
        public:
            sema_function_impl& create(const sema_context_interface& ctx, const sema_type& return_type, function_signature s);
        };

        class sema_type_factory : public factory<sema_type>
        {
        public:
            const sema_type& create(const sema_context_interface& ctx, lexer::token::token name, std::vector<member_info> members);
        };

        class sema_context_factory : public factory<sema_context_interface>
        {
        public:
            const sema_context_interface& create(const sema_context_interface* parent);
        };
    }
}

