#pragma once

#include "exec/instance/int_alias.hpp"
#include "exec/instance/generic_instance_value.hpp"
#include "exec/instance/instance.hpp"

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
                    value(generic_instance_value val) : m_generic{ std::move(val) } {}
                    ~value() {}

                    bool m_bool;
                    int_t m_int;
                    double m_double;
                    std::string m_string;
                    generic_instance_value m_generic;
                } m_value;

            public:
                enum class which_type
                {
                    bool_,
                    int_,
                    double_,
                    string,
                    generic
                };

                instance_value_variant(const instance_value_variant& other);

                instance_value_variant& operator=(const instance_value_variant& other);

                instance_value_variant(instance_value_variant&&) = default;
                instance_value_variant& operator=(instance_value_variant&&)  = default;

                explicit instance_value_variant();

                instance_value_variant(bool val);

                instance_value_variant(int_t val);

                instance_value_variant(double val);

                // Prevent conversion from const char* to bool.
                instance_value_variant(const char* value);

                instance_value_variant(std::string val);

                instance_value_variant(generic_instance_value val);
                ~instance_value_variant();

                which_type which() const;

                bool get_bool() const;

                void set_bool(bool value);

                int_t get_int() const;
                void set_int(int_t value);

                double get_double() const;

                void set_double(double value);

                const std::string& get_string_cref() const;

                void set_string(std::string value);

                const generic_instance_value& get_generic_cref() const;

                void set_generic(generic_instance_value value);

            private:
                template <typename T>
                void reassign(T&& val, which_type w);

                void assign(const value& other, which_type w);

                template <typename T>
                void assign(T&& val, which_type w);

                void construct(bool value);

                void construct(int_t value);

                void construct(double value);

                void construct(std::string value);

                void construct(generic_instance_value value);

                template <typename Value>
                void construct(Value& destination_ptr, Value&& value);

                void destruct();

                template <typename T>
                void call_dtor(T& val);

            private:
                which_type m_which;
            };
        }
    }
}
