#pragma once

#include "exec/global_executor.hpp"
#include "test/exec/mock/cmake_facade_mock.hpp"

#include <gmock/gmock.h>

namespace cmsl::exec::test
{
    class ExecutionSmokeTest : public ::testing::Test
    {
    protected:
        ::testing::NiceMock<cmake_facade_mock> m_facade;
        global_executor m_executor{"<unknown source>", m_facade};
    };
}
