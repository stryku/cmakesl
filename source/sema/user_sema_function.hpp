#pragma once

#include "sema/sema_function.hpp"

namespace cmsl
{
    namespace sema
    {
        class block_node;
        class sema_function_builder;
        class sema_context_interface;

        class user_sema_function : public sema_function
        {
        public:
            explicit user_sema_function(const sema_context_interface& ctx, const sema_type& return_type, function_signature s)
                : m_ctx{ ctx }
                , m_return_type{ return_type }
                , m_signature{ std::move(s) }
            {}

            // Todo: It probably should be const.
            void set_body(block_node& body)
            {
                m_body = &body;
            }

            const block_node& body() const
            {
                return *m_body;
            }

            const function_signature& signature() const override
            {
                return m_signature;
            }

            const sema_type& return_type() const override
            {
                return m_return_type;
            }

            const sema_context_interface& context() const override
            {
                return m_ctx;
            }

        private:
            friend class sema_function_builder;
            // A context that this function is registered in. Can be a namespace or class context.
            const sema_context_interface& m_ctx;
            const sema_type& m_return_type;
            function_signature m_signature;
            // It will be set by a function builder. It needs to be set after creation because it can refer to itself in case of a recursion.
            block_node* m_body;
        };
    }
}