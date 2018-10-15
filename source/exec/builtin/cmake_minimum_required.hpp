#pragma once

#include "cmake_facade.hpp"

namespace cmsl
{
    namespace exec
    {
        namespace builtin
        {
            class cmake_minimum_required
            {
            public:
                cmake_minimum_required(facade::cmake_facade& cmake_facade);

                void call(const facade::cmake_facade::version& version);
            };
        }
    }
}
