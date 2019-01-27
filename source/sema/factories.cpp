#include "sema/factories.hpp"
#include "sema/user_sema_function.hpp"
#include "sema/sema_type.hpp"
#include "sema/sema_context.hpp"

namespace cmsl
{
    namespace sema
    {
        user_sema_function&
        sema_function_factory::create(const sema_context_interface &ctx, const sema_type &return_type, function_signature s)
        {
            return create_impl<user_sema_function>(ctx, return_type, std::move(s));
        }

        const sema_type&
        sema_type_factory::create(const sema_context_interface &ctx, lexer::token::token name, std::vector<member_info> members)
        {
            return create_impl<sema_type>(ctx, name, std::move(members));
        }

        sema_context& sema_context_factory::create(const sema_context_interface *parent)
        {
            return create_impl<sema_context>(parent);
        }
    }
}
