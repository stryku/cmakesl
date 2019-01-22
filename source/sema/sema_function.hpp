#pragma once

#include "lexer/token/token.hpp"

namespace cmsl
{
    namespace sema
    {
        class sema_context;
        class sema_type;
        class block_node;

        class sema_function_builder;

        class sema_function
        {
        public:
            struct parameter_declaration
            {
                const sema_type& ty;
                lexer::token::token name;
            };

            struct function_signature
            {
                lexer::token::token name;
                std::vector<parameter_declaration> params;
            };

            explicit sema_function(const sema_context& ctx, const sema_type& return_type, function_signature s)
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

            const function_signature& signature() const
            {
                return m_signature;
            }

            const sema_type& return_type() const
            {
                return m_return_type;
            }

        private:
            friend class sema_function_builder;
            // A context that this function is registered in. Can be a namespace or class context.
            const sema_context& m_ctx;
            const sema_type& m_return_type;
            function_signature m_signature;
            // It will be set by a function builder. It needs to be set after creation because it can refer to itself in case of a recursion.
            block_node* m_body;
        };
    }
}