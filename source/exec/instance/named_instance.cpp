#include "named_instance.hpp"

#include "exec/execution_context.hpp"

namespace cmsl
{
    namespace exec
    {
        namespace inst
        {
            named_instance::named_instance(cmsl::string_view name,
                                           execution_context &ctx)
                : named_instance{ name, *ctx.get_variable(name) }
            {}

            named_instance::named_instance(cmsl::string_view name, instance &referenced_instance)
                : m_name{ name }
                , m_instance{ referenced_instance }
            {}

            instance_value_t named_instance::get_value() const
            {
                return m_instance.get_value();
            }

            instance_value_t& named_instance::get_value_ref()
            {
                return m_instance.get_value_ref();
            }

            const instance_value_t& named_instance::get_value_cref() const
            {
                return m_instance.get_value_cref();
            }

            void named_instance::assign(instance_value_t val)
            {
                m_instance.assign(val);
            }

            std::unique_ptr<instance> named_instance::copy() const
            {
                return m_instance.copy();
            }

            instance* named_instance::get_member(cmsl::string_view name)
            {
                return m_instance.get_member(name);
            }

            bool named_instance::has_function(cmsl::string_view name) const
            {
                return m_instance.has_function(name);
            }

            const ast::function* named_instance::get_function(cmsl::string_view name) const
            {
                return m_instance.get_function(name);
            }

            const ast::type &named_instance::get_type() const
            {
                return m_instance.get_type();
            }

            bool named_instance::is_ctor(cmsl::string_view name) const
            {
                return get_type().get_name() == name;
            }
        }
    }
}
