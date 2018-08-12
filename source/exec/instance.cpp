#include "exec/instance.hpp"

namespace cmsl
{
    namespace exec
    {
        instance::instance(const ast::type& type)
            : m_type{ type }
            , m_value{ 0u }
        {}

        int instance::get_value() const
        {
            return m_value;
        }

        void instance::assign(int val)
        {
            m_value = val;
        }
    }
}
