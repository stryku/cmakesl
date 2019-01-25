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

        class sema_function_builder
        {
        public:
            explicit sema_function_builder(const sema_context& ctx, lexer::token::token name, std::vector<sema_function::parameter_declaration> parameters)
                : m_function{ std::make_unique<sema_function_impl>(ctx, name, std::move(parameters)) }
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
            std::unique_ptr<sema_function_impl> m_function;
        };
    }
}
