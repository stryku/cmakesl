#pragma once

#include "sema/sema_function.hpp"

#include <memory>
#include <vector>

namespace cmsl
{
    namespace sema
    {
        class sema_context;
        class block_node;

        /*
        class sema_function_builder
        {
        public:
            explicit sema_function_builder(const sema_context& ctx, const sema_type& return_type, sema_function::function_signature signature)
                : m_function{ std::make_unique<user_sema_function>(ctx, return_type, std::move(signature)) }
            {}

            sema_function_builder& with_body(block_node& block)
            {
                m_function->m_body = &block;
                return *this;
            }

            std::unique_ptr<sema_function> build()
            {
                return std::move(m_function);
            }

        private:
            std::unique_ptr<user_sema_function> m_function;
        };
         */
    }
}
