#pragma once

#include "exec/global_executor.hpp"
#include "test/exec/mock/cmake_facade_mock.hpp"

#include <gmock/gmock.h>

namespace cmsl::exec::test {
class ExecutionSmokeTest : public ::testing::Test
{
protected:
  ::testing::NiceMock<cmake_facade_mock> m_facade;

  void SetUp() override
  {
    m_executor = std::make_unique<global_executor>(
      CMAKESL_EXEC_SMOKE_TEST_ROOT_DIR, m_facade);
  }

  std::unique_ptr<global_executor> m_executor;
};
}
