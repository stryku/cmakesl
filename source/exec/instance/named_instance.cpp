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
                : m_name{name}
                , m_ctx{ ctx }
                , m_instance{ m_ctx.get_variable(m_name) }
            {}

            instance_value_t named_instance::get_value() const
            {
                return m_instance->get_value();
            }

            void named_instance::assign(instance_value_t val)
            {
                m_instance->assign(val);
            }

            std::unique_ptr<instance> named_instance::copy() const
            {
                return std::make_unique<named_instance>(m_name, m_ctx);
            }

            instance* named_instance::get_member(cmsl::string_view name)
            {
                return m_instance->get_member(name);
            }

            bool named_instance::has_function(cmsl::string_view name) const
            {
                return m_instance->has_function(name);
            }

            const ast::function_node* named_instance::get_function(cmsl::string_view name) const
            {
                return m_instance->get_function(name);
            }

            const ast::type &named_instance::get_type() const
            {
                return m_instance->get_type();
            }
        }
    }
}
