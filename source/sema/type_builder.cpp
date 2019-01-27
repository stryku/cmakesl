#include "sema/type_builder.hpp"
#include "sema/factories.hpp"
#include "sema/sema_context.hpp"
#include "sema/user_sema_function.hpp"

namespace cmsl
{
    namespace sema
    {
        type_builder::type_builder(sema_type_factory &type_factory,
                                   sema_function_factory &function_factory,
                                   sema_context_factory &context_factory,
                                   sema_context &parent_ast_ctx,
                                   lexer::token::token name)
            : m_type_factory{ type_factory }
            , m_function_factory{ function_factory }
            , m_context_factory{ context_factory }
            , m_ctx{ m_context_factory.create(&parent_ast_ctx) }
            , m_name{ name }
        {}

        type_builder &type_builder::with_member(const member_info &member)
        {
            m_members.emplace_back(member);
            return *this;
        }

        type_builder &type_builder::with_function(const sema_type &return_type, function_signature s)
        {
            const auto& function = m_function_factory.create(m_ctx, return_type, std::move(s));
            m_ctx.add_function(function);
            return *this;
        }

        const sema_type& type_builder::build()
        {
            return m_type_factory.create(m_ctx, m_name, std::move(m_members));
        }

        const sema_context_interface& type_builder::context()
        {
            return m_ctx;
        }
    }
}