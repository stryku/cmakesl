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
                MOCK_CONST_METHOD1(fatal_error, void(const std::string&));
                MOCK_METHOD1(register_project, void(const std::string&));
            };
        }
    }
}
