#include "test/exec/smoke_test_fixture.hpp"

#include <gmock/gmock.h>

namespace cmsl::exec::test {
using ::testing::Eq;

using CmakeNamespaceSmokeTest = ExecutionSmokeTest;

TEST_F(CmakeNamespaceSmokeTest, CanCallMessage)
{
  const auto source = "int main()"
                      "{"
                      "    cmake::message(\"foo\");"
                      "    return 42;"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(CmakeNamespaceSmokeTest, CanCallWarning)
{
  const auto source = "int main()"
                      "{"
                      "    cmake::warning(\"foo\");"
                      "    return 42;"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(CmakeNamespaceSmokeTest, CanCallError)
{
  const auto source = "int main()"
                      "{"
                      "    cmake::error(\"foo\");"
                      "    return 42;"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(CmakeNamespaceSmokeTest, CanCallFatalError)
{
  const auto source = "int main()"
                      "{"
                      "    cmake::fatal_error(\"foo\");"
                      "    return 42;"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(42));
}
}
