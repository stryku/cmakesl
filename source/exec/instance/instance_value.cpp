#include "exec/instance/instance_value.hpp"

namespace cmsl
{
    namespace exec
    {
        namespace inst
        {
            generic::generic(const generic& other)
                    : value_type{ other.value_type }
                    , m_type{ other.m_type }
            {}

            generic& generic::operator=(const generic& other)
            {
                value_type = other.value_type;
            }

            bool generic::operator==(const generic& rhs) const
            {
                return false;
            }
        }
    }
}

namespace std
{
    std::ostream &operator<<(std::ostream &out, const cmsl::exec::inst::generic &)
    {
        return out;
    }
}
