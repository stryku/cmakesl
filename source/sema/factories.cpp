#include "sema/factories.hpp"

namespace cmsl
{
    namespace sema
    {
        user_sema_function&
        sema_function_factory::create(const sema_context_interface &ctx, const sema_type &return_type, function_signature s)
        {
            return create_impl(ctx, return_type, std::move(s));
        }

        const sema_type&
        sema_type_factory::create(const sema_context_interface &ctx, lexer::token::token name, std::vector<member_info> members)
        {
            return create_impl(ctx, name, std::move(members));
        }

        const sema_context_interface& sema_context_factory::create(const sema_context_interface *parent)
        {
            return create_impl(parent);
        }
    }
}
