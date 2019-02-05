#include "exec/instance/instance_value_variant.hpp"
#include "exec/instance/generic_instance_value.hpp"
#include "exec/instance/instance.hpp"

namespace cmsl
{
    namespace exec
    {
        namespace inst
        {
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
                assign(val, which_type::bool_);
            }

            instance_value_variant::instance_value_variant(int_t val)
            {
                assign(val, which_type::int_);
            }

            instance_value_variant::instance_value_variant(double val)
            {
                assign(val, which_type::double_);
            }

            // Prevent conversion from const char* to bool.
            instance_value_variant::instance_value_variant(const char* value)
                : instance_value_variant{ std::string{ value } }
            {}

            instance_value_variant::instance_value_variant(std::string val)
            {
                assign(std::move(val), which_type::string);
            }

            instance_value_variant::instance_value_variant(generic_instance_value val)
            {
                assign(std::move(val), which_type::generic);
            }

            instance_value_variant::~instance_value_variant()
            {
                destruct();
            }

            instance_value_variant::which_type instance_value_variant::which() const
            {
                return m_which;
            }

            bool instance_value_variant::get_bool() const
            {
                return m_value.m_bool;
            }

            void instance_value_variant::set_bool(bool value)
            {
                reassign(value, which_type::bool_);
            }

            int_t instance_value_variant::get_int() const
            {
                return m_value.m_int;
            }

            void instance_value_variant::set_int(int_t value)
            {
                reassign(value, which_type::int_);
            }

            double instance_value_variant::get_double() const
            {
                return m_value.m_double;
            }

            void instance_value_variant::set_double(double value)
            {
                reassign(value, which_type::double_);
            }

            const std::string& instance_value_variant::get_string_cref() const
            {
                return m_value.m_string;
            }

            void instance_value_variant::set_string(std::string value)
            {
                reassign(std::move(value), which_type::string);
            }

            const generic_instance_value& instance_value_variant::get_generic_cref() const
            {
                return m_value.m_generic;
            }

            void instance_value_variant::set_generic(generic_instance_value value)
            {
                reassign(std::move(value), which_type::generic);
            }

            template <typename T>
            void instance_value_variant::reassign(T&& val, which_type w)
            {
                destruct();
                assign(std::forward<T>(val), w);
            }

            void instance_value_variant::assign(const value& other, which_type w)
            {
                switch (w)
                {
                    case which_type::bool_:
                    {
                        assign(other.m_bool, w);
                    }break;
                    case which_type::int_:
                    {
                        assign(other.m_int, w);
                    }break;
                    case which_type::double_:
                    {
                        assign(other.m_double, w);
                    }break;
                    case which_type::string:
                    {
                        assign(other.m_string, w);
                    }break;
                    case which_type::generic:
                    {
                        assign(other.m_generic, w);
                    }break;
                }
            }

            template <typename T>
            void instance_value_variant::assign(T&& val, which_type w)
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

            void instance_value_variant::construct(generic_instance_value value)
            {
                construct(m_value.m_generic, std::move(value));
            }

            template <typename Value>
            void instance_value_variant::construct(Value& destination_ptr, Value&& value)
            {
                new (&destination_ptr) Value{ std::move(value) };
            }

            void instance_value_variant::destruct()
            {
                switch (m_which)
                {
                    case which_type::bool_:
                    case which_type::int_:
                    case which_type::double_:
                        // Primitives, do nothing.
                        break;
                    case which_type::string:
                    {
                        call_dtor(m_value.m_string);
                    } break;
                    case which_type::generic:
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
        }
    }
}

