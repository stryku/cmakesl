#include "sema/sema_type.hpp"
#include "sema/sema_context.hpp"

namespace cmsl
{
    namespace sema
    {
        sema_type::sema_type(const sema_context_interface& ctx, token_t name, std::vector<member_info> members)
                : m_ctx{ ctx }
                , m_name{ name }
                , m_members{ std::move(members) }
        {}

        sema_type::token_t sema_type::name() const
        {
            return m_name;
        }

        const sema_context_interface& sema_type::context() const
        {
            return m_ctx;
        }

        boost::optional<sema_type::member_info> sema_type::find_member(cmsl::string_view name) const
        {
            const auto pred = [name](const auto& member)
            {
                return member.name.str() == name;
            };

            auto found = std::find_if(std::cbegin(m_members), std::cend(m_members), pred);
            if(found == std::cend(m_members))
            {
                return {};
            }

            return *found;
        }

        const sema_function* sema_type::find_member_function(cmsl::string_view name) const
        {
            return m_ctx.find_function_in_this_scope(name);
        }

        bool sema_type::operator==(const sema_type& rhs) const
        {
            return this == &rhs;
        }

        bool sema_type::operator!=(const sema_type &rhs) const
        {
            return !(*this == rhs);
        }
    }
}
