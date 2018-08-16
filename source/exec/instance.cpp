#include "exec/instance.hpp"

namespace cmsl
{
    namespace exec
    {
        instance::instance(const ast::type& type)
            : m_type{ type }
            , m_value{ 0u }
        {}

        instance::instance(const ast::type& type, int value)
            : m_type{ type }
            , m_value{ value }
        {}

        int instance::get_value() const
        {
            return m_value;
        }

        void instance::assign(int val)
        {
            m_value = val;
        }

        instance* instance::get_member(cmsl::string_view name)
        {
            auto found = m_members.find(name);

            return found != std::end(m_members)
                ? found->second.get()
                : nullptr;
        }
    }
}
