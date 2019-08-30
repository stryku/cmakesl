#include "test/exec/smoke_test_fixture.hpp"

#include <gmock/gmock.h>

namespace cmsl::exec::test {
using ::testing::Eq;

using BuiltinIdentifiersSmokeTest = ExecutionSmokeTest;

TEST_F(BuiltinIdentifiersSmokeTest, Cmake_ModulePath)
{
  const auto source = "int main()"
                      "{"
                      "    return int(cmake::module_path.empty());"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(1));
}
}
