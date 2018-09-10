#include "cmake_facade.hpp"

#include <cmVersion.h>

namespace cmsl
{
    namespace facade
    {
        cmake_facade::version cmake_facade::get_cmake_version() const
        {
            return {
                    cmVersion::GetMajorVersion(),
                    cmVersion::GetMinorVersion(),
                    cmVersion::GetPatchVersion(),
                    cmVersion::GetTweakVersion()
            };
        }
    }
}
