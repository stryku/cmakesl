#pragma once

#include "cmake_facade.hpp"

#include <gmock/gmock.h>

namespace cmsl
{
    namespace exec
    {
        namespace test
        {
            class cmake_facade_mock : public facade::cmake_facade
            {
            public:
                MOCK_CONST_METHOD0(get_cmake_version, version());
            };
        }
    }
}
