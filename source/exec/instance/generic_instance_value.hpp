#pragma once

#include <boost/variant.hpp>

#include <string>
#include <list>
#include <memory>

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
            class instance;

            class generic_instance_value
            {
            public:
                using instance_ptr_t = std::unique_ptr<instance>;
                using list_t = std::list<instance_ptr_t>;
                using generic_variant_t = boost::variant<list_t>;

                enum class generic_instance_value_type
                {
                    list
                };

                explicit generic_instance_value(const ast::type& t, generic_instance_value_type value_type);

                generic_instance_value(const generic_instance_value& other);
                generic_instance_value& operator=(const generic_instance_value& other);

                bool operator==(const generic_instance_value& rhs) const;

                const ast::type& get_type() const;

                template <typename Callable>
                auto apply(Callable&& cb)
                {
                    return boost::apply_visitor(cb, m_generic_value);
                }

                template <typename Callable>
                auto apply(Callable&& cb) const
                {
                    return boost::apply_visitor(cb, m_generic_value);
                }

            private:
                generic_variant_t get_init_value() const;
                void copy_value(const generic_instance_value& other);

            private:
                const ast::type& m_type;
                generic_instance_value_type m_value_type;
                generic_variant_t m_generic_value;
            };
        }
    }
}

namespace std
{
    std::ostream &operator<<(std::ostream &out, const cmsl::exec::inst::generic_instance_value &);
}
