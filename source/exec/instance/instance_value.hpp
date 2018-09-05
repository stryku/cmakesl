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
//            using instance_value_t = boost::variant<bool, int, double, std::string>;

            class instance_value;

            enum class instance_value_type
            {
                bool_,
                int_,
                double_,
                string,
                list
            };

            class generic
            {
            public:
                using value_ptr_t = std::unique_ptr<instance_value>;
                using list_t = std::list<value_ptr_t>;
                using generic_variant_t = boost::variant<list_t>;

                instance_value_type value_type;
                generic_variant_t generic_value;
                const ast::type& m_type;

                generic(const generic& other);
                generic& operator=(const generic& other);

                bool operator==(const generic& rhs) const;
            };

            using instance_value_t = boost::variant<bool, int, double, std::string, generic>;

            // Wrapper that helps with forward declaration. generic stores it as unique_ptr. Thanks to that it can be chained -> generics of generics and so on.
            struct instance_value
            {
                instance_value_t val;
            };
        }
    }
}

namespace std
{
    std::ostream &operator<<(std::ostream &out, const cmsl::exec::inst::generic &);
}
