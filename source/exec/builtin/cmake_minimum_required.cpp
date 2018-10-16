#include "exec/builtin/cmake_minimum_required.hpp"

namespace cmsl
{
    namespace exec
    {
        namespace builtin
        {
            cmake_minimum_required::cmake_minimum_required(facade::cmake_facade &cmake_facade)
                : m_facade{ cmake_facade }
            {}

            void cmake_minimum_required::call(const facade::cmake_facade::version &version)
            {
                const auto current_cmake_version = m_facade.get_cmake_version();
                if(current_cmake_version < version)
                {
                    m_facade.fatal_error(""); // todo use errors_observer
                }
            }
        }
    }
}
