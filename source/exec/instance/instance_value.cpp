#include "exec/instance/instance_value.hpp"

namespace cmsl
{
    namespace exec
    {
        namespace inst
        {
            generic_instance_value::generic_instance_value(const ast::type &t, instance_value_type value_type)
                : m_type{ t }
                , m_value_type{ value_type }
                , m_generic_value{ get_init_value() }
            {}

            generic_instance_value::generic_instance_value(const generic_instance_value& other)
                : m_value_type{ other.m_value_type }
                , m_type{ other.m_type }
            {}

            generic_instance_value& generic_instance_value::operator=(const generic_instance_value& other)
            {
                m_value_type = other.m_value_type;
            }

            bool generic_instance_value::operator==(const generic_instance_value& rhs) const
            {
                return false;
            }

            generic_instance_value::generic_variant_t generic_instance_value::get_init_value() const
            {
                switch(m_value_type)
                {
                    case instance_value_type::list: return list_t{};
                }
            }

            const ast::type &generic_instance_value::get_type() const
            {
                return m_type;
            }
        }
    }
}

namespace std
{
    std::ostream &operator<<(std::ostream &out, const cmsl::exec::inst::generic_instance_value &)
    {
        return out;
    }
}
