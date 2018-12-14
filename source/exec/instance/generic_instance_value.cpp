#include "exec/instance/generic_instance_value.hpp"
#include "exec/instance/instance.hpp"

namespace cmsl
{
    namespace exec
    {
        namespace inst
        {
            generic_instance_value::generic_instance_value(const ast::type &t, generic_instance_value::generic_instance_value_type value_type)
                    : m_type{ t }
                    , m_value_type{ value_type }
                    , m_generic_value{ get_init_value() }
            {}
            generic_instance_value::generic_instance_value(const ast::type &t, generic_instance_value_type value_type, generic_variant_t initial_value)
                    : m_type{ t }
                    , m_value_type{ value_type }
                    , m_generic_value{ std::move(initial_value) }
            {}

            generic_instance_value::generic_instance_value(const generic_instance_value& other)
                    : m_value_type{ other.m_value_type }
                    , m_type{ other.m_type }
            {
                copy_value(other);
            }

            generic_instance_value& generic_instance_value::operator=(const generic_instance_value& other)
            {
                m_value_type = other.m_value_type;
                copy_value(other);
            }

            bool generic_instance_value::operator==(const generic_instance_value& rhs) const
            {
                return false;
            }

            generic_instance_value::generic_variant_t generic_instance_value::get_init_value() const
            {
                switch(m_value_type)
                {
                    case generic_instance_value_type::list: return list_t{};
                }
            }

            const ast::type &generic_instance_value::get_type() const
            {
                return m_type;
            }

            void generic_instance_value::copy_value(const generic_instance_value& other)
            {
                switch(other.m_value_type)
                {
                    case generic_instance_value_type::list:
                    {
                        auto& list = boost::get<list_t>(other.m_generic_value);
                        auto my_list = list_t{};
                        for(const auto& val : list)
                        {
                            my_list.push_back(val->copy());
                        }
                        m_generic_value = std::move(my_list);
                    }break;
                }
            }

            const generic_instance_value::generic_instance_value_type generic_instance_value::get_value_type() const
            {
                return m_value_type;
            }

            const generic_instance_value::generic_variant_t& generic_instance_value::get_underlying_value_cref() const
            {
                return m_generic_value;
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