#pragma once

#include "errors/errors_observer.hpp"
#include "exec/global_executor.hpp"
#include "test/errors_observer_mock/errors_observer_mock.hpp"
#include "test/mock/cmake_facade_mock.hpp"

#include <gmock/gmock.h>

namespace cmsl::exec::test {
class ExecutionSmokeTest : public ::testing::Test
{
protected:
  ::testing::NiceMock<cmake_facade_mock> m_facade;

  void SetUp() override
  {
    m_errors_observer_mock =
      std::make_unique<errors::test::errors_observer_mock>();

    m_executor = std::make_unique<global_executor>(
      CMAKESL_EXEC_SMOKE_TEST_ROOT_DIR, m_facade, *m_errs);
  }

  void TearDown() override
  {
    m_executor.reset();
    m_errors_observer_mock.reset();
  }

  std::unique_ptr<errors::test::errors_observer_mock> m_errors_observer_mock;
  std::unique_ptr<errors::errors_observer> m_errs;
  std::unique_ptr<global_executor> m_executor;
};
}
