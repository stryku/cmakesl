#include "exec/builtin/cmake_minimum_required.hpp"

#include "exec/instance/instances_holder.hpp"

namespace cmsl
{
    namespace exec
    {
        namespace builtin
        {
            cmake_minimum_required::cmake_minimum_required(facade::cmake_facade &cmake_facade, inst::instances_holder_interface& instances, facade::cmake_facade::version version)
                : evaluatable_function{ instances }
                , m_facade{ cmake_facade }
                , m_version{ version }
            {}

            inst::instance *cmake_minimum_required::evaluate()
            {
                const auto current_cmake_version = m_facade.get_cmake_version();
                if(current_cmake_version < m_version)
                {
                    m_facade.fatal_error(""); // todo use errors_observer
                }

                return m_instances.create_void();
            }
        }
    }
}
