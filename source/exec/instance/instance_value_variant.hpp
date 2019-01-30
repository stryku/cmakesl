#pragma once

#include "exec/instance/int_alias.hpp"

#include <string>

namespace cmsl
{
    namespace exec
    {
        namespace inst
        {
            class instance_value_variant
            {
            private:
                union value
                {
                    value() : m_bool{ false } {}
                    value(bool val) : m_bool{ val } {}
                    value(int_t val) : m_int{ val } {}
                    value(double val) : m_double{ val } {}
                    value(std::string val) : m_string{ std::move(val) } {}
                    ~value() {}

                    bool m_bool;
                    int_t m_int;
                    double m_double;
                    std::string m_string;
                } m_value;

            public:
                enum class which_type
                {
                    bool_,
                    int_,
                    double_,
                    string
                };

                instance_value_variant(const instance_value_variant& other)
                {
                    reassign(other.m_value, other.m_which);
                }

                instance_value_variant& operator=(const instance_value_variant& other)
                {
                    if(this != &other)
                    {
                        reassign(other.m_value, other.m_which);
                    }

                    return *this;
                }

                instance_value_variant(instance_value_variant&&) = default;
                instance_value_variant& operator=(instance_value_variant&&)  = default;

                explicit instance_value_variant()
                        : instance_value_variant{ false }
                {}

                instance_value_variant(bool val)
                {
                    assign(val, which_type::bool_);
                }

                instance_value_variant(int_t val)
                {
                    assign(val, which_type::int_);
                }

                instance_value_variant(double val)
                {
                    assign(val, which_type::double_);
                }

                // Prevent conversion from const char* to bool.
                instance_value_variant(const char* value)
                    : instance_value_variant{ std::string{ value } }
                {}

                instance_value_variant(std::string val)
                {
                    assign(std::move(val), which_type::string);
                }

                ~instance_value_variant()
                {
                    destruct();
                }

                which_type which() const
                {
                    return m_which;
                }

                bool get_bool() const
                {
                    return m_value.m_bool;
                }

                void set_bool(bool value)
                {
                    reassign(value, which_type::bool_);
                }

                int_t get_int() const
                {
                    return m_value.m_int;
                }

                void set_int(int_t value)
                {
                    reassign(value, which_type::int_);
                }

                double get_double() const
                {
                    return m_value.m_double;
                }

                void set_double(double value)
                {
                    reassign(value, which_type::double_);
                }

                const std::string& get_string_cref() const
                {
                    return m_value.m_string;
                }

                void set_string(std::string value)
                {
                    reassign(std::move(value), which_type::string);
                }

            private:
                template <typename T>
                void reassign(T&& val, which_type w)
                {
                    destruct();
                    assign(std::forward<T>(val), w);
                }

                void assign(const value& other, which_type w)
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
                    }
                }

                template <typename T>
                void assign(T&& val, which_type w)
                {
                    m_which  = w;
                    construct(std::forward<T>(val));
                }

                void construct(bool value)
                {
                    m_value.m_bool = value;
                }

                void construct(int_t value)
                {
                    m_value.m_int = value;
                }

                void construct(double value)
                {
                    m_value.m_double = value;
                }

                void construct(std::string value)
                {
                    new (&m_value.m_string) std::string{ std::move(value) };
                }

                void destruct()
                {
                    switch (m_which)
                    {
                        case which_type::string:
                        {
                            m_value.m_string.std::string::~string();
                        }
                            break;

                        case which_type::bool_:
                        case which_type::int_:
                        case which_type::double_:
                            // Primitives, do nothing.
                            break;
                    }
                }

            private:
                which_type m_which;
            };
        }
    }
}
