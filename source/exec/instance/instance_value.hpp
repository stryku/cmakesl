#pragma once

#include <boost/variant.hpp>

#include <string>
#include <list>

namespace cmsl
{
    namespace ast
    {
        class type;
    }

    namespace exec
    {
        namespace inst
        {
            class instance_value_wrapper;

            enum class instance_value_type
            {
                bool_,
                int_,
                double_,
                string,
                list
            };

            class generic_instance_value
            {
            public:
                using value_ptr_t = std::unique_ptr<instance_value_wrapper>;
                using list_t = std::list<value_ptr_t>;
                using generic_variant_t = boost::variant<list_t>;

                explicit generic_instance_value(const ast::type& t, instance_value_type value_type);

                generic_instance_value(const generic_instance_value& other);
                generic_instance_value& operator=(const generic_instance_value& other);

                bool operator==(const generic_instance_value& rhs) const;

                const ast::type& get_type() const;

            private:
                generic_variant_t get_init_value() const;

            private:
                const ast::type& m_type;
                instance_value_type m_value_type;
                generic_variant_t m_generic_value;
            };

            using instance_value_t = boost::variant<bool, int, double, std::string, generic_instance_value>;

            // Wrapper that helps with forward declaration. generic stores it as unique_ptr. Thanks to that it can be chained -> generics of generics and so on.
            struct instance_value_wrapper
            {
                instance_value_t val;
            };
        }
    }
}

namespace std
{
    std::ostream &operator<<(std::ostream &out, const cmsl::exec::inst::generic_instance_value &);
}
