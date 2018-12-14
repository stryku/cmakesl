#include "exec/builtin/container.hpp"

#include "exec/instance/instances_holder_interface.hpp"
#include "exec/instance/instance.hpp"
#include "exec/builtin/builtin_function_visitors.hpp"

namespace cmsl
{
    namespace exec
    {
        namespace builtin
        {
            container::container(inst::instance &class_instance, inst::instances_holder_interface& instances)
                : m_class_instance{ class_instance }
                , m_instances{ instances }
            {}

            inst::instance *container::empty()
            {
                const auto& val = m_class_instance.get_value_ref();
                const auto result = empty_visitor{}.visit(val);
                return m_instances.create(result);
            }

            inst::instance *container::size()
            {
                const auto& val = m_class_instance.get_value_ref();
                const auto result = size_visitor{}.visit(val);
                return m_instances.create(result);
            }
        }
    }
}
