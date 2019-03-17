#include "exec/instance/instance_value_variant.hpp"
#include "exec/instance/generic_instance_value.hpp"
#include "exec/instance/instance.hpp"
#include "instance_value_variant.hpp"
#include "common/assert.hpp"


namespace cmsl
{
    namespace exec
    {
        namespace inst
        {
            instance_value_variant::instance_value_variant(instance_value_variant && other)
            {
                move_from(std::move(other));
            }

            instance_value_variant &instance_value_variant::operator=(instance_value_variant && other)
            {
                if(this != &other)
                {
                    move_from(std::move(other));
                }

                return *this;
            }

            void instance_value_variant::move_from(instance_value_variant&& moved)
            {
                switch (moved.m_which)
                {
                    case which_t::bool_:
                    {
                        reassign(moved.m_value.m_bool, which_t::bool_);
                    }break;
                    case which_t::int_:
                    {
                        reassign(moved.m_value.m_int, which_t::int_);
                    }break;
                    case which_t::double_:
                    {
                        reassign(moved.m_value.m_double, which_t::double_);
                    }break;
                    case which_t::string:
                    {
                        reassign(std::move(moved.m_value.m_string), which_t::string);
                    }break;
                    case which_t::version:
                    {
                        reassign(std::move(moved.m_value.m_version), which_t::version);
                    }break;
                    case which_t::list:
                    {
                        reassign(std::move(moved.m_value.m_list), which_t::version);
                    }break;
                    case which_t::generic:
                    {
                        reassign(std::move(moved.m_value.m_generic), which_t::generic);
                    }break;
                }
            }

            instance_value_variant::instance_value_variant(const instance_value_variant& other)
            {
                reassign(other.m_value, other.m_which);
            }

            instance_value_variant& instance_value_variant::operator=(const instance_value_variant& other)
            {
                if(this != &other)
                {
                    reassign(other.m_value, other.m_which);
                }

                return *this;
            }

            instance_value_variant::instance_value_variant()
                    : instance_value_variant{ false }
            {}

            instance_value_variant::instance_value_variant(bool val)
            {
                assign(val, which_t::bool_);
            }

            instance_value_variant::instance_value_variant(int_t val)
            {
                assign(val, which_t::int_);
            }

            instance_value_variant::instance_value_variant(double val)
            {
                assign(val, which_t::double_);
            }

            // Prevent conversion from const char* to bool.
            instance_value_variant::instance_value_variant(const char* value)
                : instance_value_variant{ std::string{ value } }
            {}

            instance_value_variant::instance_value_variant(std::string val)
            {
                assign(std::move(val), which_t::string);
            }

            instance_value_variant::instance_value_variant(version_value val)
            {
                assign(std::move(val), which_t::version);
            }

            instance_value_variant::instance_value_variant(list_value val)
            {
                assign(std::move(val), which_t::list);
            }

            instance_value_variant::instance_value_variant(generic_instance_value val)
            {
                assign(std::move(val), which_t::generic);
            }

            instance_value_variant::~instance_value_variant()
            {
                destruct();
            }

            instance_value_variant::which_t instance_value_variant::which() const
            {
                return m_which;
            }

            bool instance_value_variant::get_bool() const
            {
                return m_value.m_bool;
            }

            void instance_value_variant::set_bool(bool value)
            {
                reassign(value, which_t::bool_);
            }

            int_t instance_value_variant::get_int() const
            {
                return m_value.m_int;
            }

            void instance_value_variant::set_int(int_t value)
            {
                reassign(value, which_t::int_);
            }

            double instance_value_variant::get_double() const
            {
                return m_value.m_double;
            }

            void instance_value_variant::set_double(double value)
            {
                reassign(value, which_t::double_);
            }

            const std::string& instance_value_variant::get_string_cref() const
            {
                return m_value.m_string;
            }

            std::string &instance_value_variant::get_string_ref()
            {
                return m_value.m_string;
            }

            void instance_value_variant::set_string(std::string value)
            {
                reassign(std::move(value), which_t::string);
            }

            const generic_instance_value& instance_value_variant::get_generic_cref() const
            {
                return m_value.m_generic;
            }

            generic_instance_value& instance_value_variant::get_generic_ref()
            {
                return m_value.m_generic;
            }

            void instance_value_variant::set_generic(generic_instance_value value)
            {
                reassign(std::move(value), which_t::generic);
            }

            template <typename T>
            void instance_value_variant::reassign(T&& val, which_t w)
            {
                destruct();
                assign(std::forward<T>(val), w);
            }

            void instance_value_variant::assign(const value& other, which_t w)
            {
                switch (w)
                {
                    case which_t::bool_:
                    {
                        assign(other.m_bool, w);
                    }break;
                    case which_t::int_:
                    {
                        assign(other.m_int, w);
                    }break;
                    case which_t::double_:
                    {
                        assign(other.m_double, w);
                    }break;
                    case which_t::string:
                    {
                        assign(other.m_string, w);
                    }break;
                    case which_t::version:
                    {
                        assign(other.m_version, w);
                    }break;
                    case which_t::list:
                    {
                        list_value list_copy = other.m_list;
                        assign(std::move(list_copy), w);
                    }break;
                    case which_t::generic:
                    {
                        assign(other.m_generic, w);
                    }break;
                }
            }

            template <typename T>
            void instance_value_variant::assign(T&& val, which_t w)
            {
                m_which  = w;
                construct(std::forward<T>(val));
            }

            void instance_value_variant::construct(bool value)
            {
                m_value.m_bool = value;
            }

            void instance_value_variant::construct(int_t value)
            {
                m_value.m_int = value;
            }

            void instance_value_variant::construct(double value)
            {
                m_value.m_double = value;
            }

            void instance_value_variant::construct(std::string value)
            {
                construct(m_value.m_string, std::move(value));
            }

            void instance_value_variant::construct(version_value value)
            {
                construct(m_value.m_version, std::move(value));
            }

            void instance_value_variant::construct(list_value value)
            {
                construct(m_value.m_list, std::move(value));
            }

            void instance_value_variant::construct(generic_instance_value value)
            {
                construct(m_value.m_generic, std::move(value));
            }

            template <typename Value>
            void instance_value_variant::construct(Value& destination, Value&& value)
            {
                new (&destination) Value{ std::move(value) };
            }

            void instance_value_variant::destruct()
            {
                switch (m_which)
                {
                    case which_t::bool_:
                    case which_t::int_:
                    case which_t::double_:
                    case which_t::version:
                        // Trivially destructible. Do nothing.
                        break;
                    case which_t::string:
                    {
                        call_dtor(m_value.m_string);
                    } break;
                    case which_t::list:
                    {
                        call_dtor(m_value.m_list);
                    } break;
                    case which_t::generic:
                    {
                        call_dtor(m_value.m_generic);
                    } break;
                }
            }

            template <typename T>
            void instance_value_variant::call_dtor(T& val)
            {
                val.~T();
            }

#define BINARY_OPERATOR(op) \
if(m_which != rhs.which()) \
{ \
    return false; \
} \
 \
switch (m_which) \
{ \
    case which_t::bool_: return get_bool() op rhs.get_bool(); \
    case which_t::int_: return get_int() op rhs.get_int(); \
    case which_t::double_:  return get_double() op rhs.get_double(); \
    case which_t::string: return get_string_cref() op rhs.get_string_cref(); \
    case which_t::version: return get_version_cref() op rhs.get_version_cref(); \
    case which_t::list: return get_list_cref() op rhs.get_list_cref(); \
    case which_t::generic: return get_generic_cref() op rhs.get_generic_cref(); \
} \
CMSL_UNREACHABLE("Unknown alternative")

            bool instance_value_variant::operator==(const instance_value_variant &rhs) const
            {
                BINARY_OPERATOR(==);
            }

            bool instance_value_variant::operator!=(const instance_value_variant &rhs) const
            {
                return !(*this == rhs);
            }

            bool instance_value_variant::operator<(const instance_value_variant &rhs) const
            {
                BINARY_OPERATOR(<);
            }

            bool instance_value_variant::operator<=(const instance_value_variant &rhs) const
            {
                return *this == rhs || *this < rhs;
            }

            bool instance_value_variant::operator>(const instance_value_variant &rhs) const
            {
                return !(*this <= rhs);
            }

            bool instance_value_variant::operator>=(const instance_value_variant &rhs) const
            {
                return *this == rhs || *this > rhs;
            }

            const version_value &instance_value_variant::get_version_cref() const
            {
                return m_value.m_version;
            }

            version_value &instance_value_variant::get_version_ref()
            {
                return m_value.m_version;
            }

            void instance_value_variant::set_version(version_value value)
            {
                reassign(std::move(value), which_t::version);
            }

            const list_value &instance_value_variant::get_list_cref() const
            {
                return m_value.m_list;
            }

            list_value &instance_value_variant::get_list_ref()
            {
                return m_value.m_list;
            }

            void instance_value_variant::set_list(list_value value)
            {
                reassign(std::move(value), which_t::list);
            }
        }
    }
}

