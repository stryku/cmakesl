#pragma once

#include "exec/instance/int_alias.hpp"
#include "exec/instance/generic_instance_value.hpp"
#include "exec/instance/version_value.hpp"
#include "exec/instance/list_value.hpp"
#include "exec/instance/instance_value_alternative.hpp"

#include <string>
#include <vector>

namespace cmsl
{
    namespace exec
    {
        namespace inst
        {
            class instance;

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
                    value(version_value val) : m_version{ std::move(val) } {}
                    value(list_value val) : m_list{ std::move(val) } {}
                    ~value() {}

                    bool m_bool;
                    int_t m_int;
                    double m_double;
                    std::string m_string;
                    version_value m_version;
                    generic_instance_value m_generic;
                    list_value m_list;
                } m_value;

            public:
                using which_t = instance_value_alternative;

                instance_value_variant(const instance_value_variant& other);
                instance_value_variant& operator=(const instance_value_variant& other);

                instance_value_variant(instance_value_variant&&other);
                instance_value_variant& operator=(instance_value_variant&&other);

                explicit instance_value_variant();

                instance_value_variant(bool val);

                instance_value_variant(int_t val);

                instance_value_variant(double val);

                // Prevent conversion from const char* to bool.
                instance_value_variant(const char* value);
                instance_value_variant(std::string val);

                instance_value_variant(version_value val);

                instance_value_variant(list_value val);

                instance_value_variant(generic_instance_value val);
                ~instance_value_variant();

                which_t which() const;

                bool get_bool() const;
                void set_bool(bool value);

                int_t get_int() const;
                void set_int(int_t value);

                double get_double() const;
                void set_double(double value);

                const std::string& get_string_cref() const;
                std::string& get_string_ref();
                void set_string(std::string value);

                const version_value& get_version_cref() const;
                version_value& get_version_ref();
                void set_version(version_value value);

                const list_value& get_list_cref() const;
                list_value& get_list_ref();
                void set_list(list_value value);

                const generic_instance_value& get_generic_cref() const;
                generic_instance_value& get_generic_ref();
                void set_generic(generic_instance_value value);

                template <typename Visitor>
                auto visit(Visitor&& visitor)
                {
                    switch(m_which)
                    {
                        case which_t::bool_: return visitor(get_bool());
                        case which_t::int_: return visitor(get_int());
                        case which_t::double_: return visitor(get_double());
                        case which_t::string: return visitor(get_string_cref());
                        case which_t::version: return visitor(get_version_cref());
                        case which_t::list: return visitor(get_list_cref());
                        case which_t::generic: return visitor(get_generic_cref());
                    }
                }

                // Todo: move to tests. It's not needed in production code.
                bool operator==(const instance_value_variant& rhs) const;
                bool operator!=(const instance_value_variant& rhs) const;
                bool operator<(const instance_value_variant& rhs) const;
                bool operator<=(const instance_value_variant& rhs) const;
                bool operator>(const instance_value_variant& rhs) const;
                bool operator>=(const instance_value_variant& rhs) const;

            private:
                template <typename T>
                void reassign(T&& val, which_t w);

                template <typename T>
                void assign(T&& val, which_t w);
                void assign(const value& other, which_t w);
                void assign(value&& other, which_t w);

                template <typename Value>
                void construct(Value& destination_ptr, Value&& value);
                void construct(bool value);
                void construct(int_t value);
                void construct(double value);
                void construct(std::string value);
                void construct(version_value value);
                void construct(list_value value);
                void construct(generic_instance_value value);

                template <typename T>
                void call_dtor(T& val);
                void destruct();

                void move_from(instance_value_variant&& moved);

            private:
                which_t m_which{ which_t::bool_ };
            };
        }
    }
}
