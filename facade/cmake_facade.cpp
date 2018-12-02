#include "cmake_facade.hpp"

#include <cmVersion.h>
#include <cmMakefile.h>

namespace cmsl
{
    namespace facade
    {
        cmake_facade::cmake_facade()
        {

        }

        cmake_facade::~cmake_facade()
        {}

        cmake_facade::version cmake_facade::get_cmake_version() const
        {
            return {
                    cmVersion::GetMajorVersion(),
                    cmVersion::GetMinorVersion(),
                    cmVersion::GetPatchVersion(),
                    cmVersion::GetTweakVersion()
            };
        }

        void cmake_facade::register_project(const std::string &name)
        {

        }

    }
}
