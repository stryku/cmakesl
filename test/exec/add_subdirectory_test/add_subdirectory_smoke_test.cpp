#include "test/exec/smoke_test_fixture.hpp"

#include <gmock/gmock.h>

namespace cmsl::exec::test {
using ::testing::Eq;
using ::testing::Return;

using AddSubdirectorySmokeTest = ExecutionSmokeTest;

TEST_F(AddSubdirectorySmokeTest, SimpleAddSubdirectory)
{
  const auto source = "int main()"
                      "{"
                      "    auto answer = add_subdirectory(\"foo\", 4.2);"
                      "    return answer;"
                      "}";

  // One while parsing and one while execution.
  EXPECT_CALL(m_facade, go_into_subdirectory("foo")).Times(2);

  EXPECT_CALL(m_facade, current_directory())
    .WillRepeatedly(Return(CMAKESL_EXEC_SMOKE_TEST_ROOT_DIR +
                           std::string{ "/add_subdirectory_test/foo" }));

  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(AddSubdirectorySmokeTest,
       OneVariableInstanceDespiteMultipleImportsWhileAddSubdirectory)
{
  const auto source = "import \"add_subdirectory_test/import/foo.cmsl\";"
                      ""
                      "int main()"
                      "{"
                      "    foo::bar *= 100.0;"
                      "    add_subdirectory(\"import\");"
                      "    return int(foo::bar);"
                      "}";

  // One while parsing and one while execution.
  EXPECT_CALL(m_facade, go_into_subdirectory("import")).Times(2);

  EXPECT_CALL(m_facade, current_directory())
    .WillRepeatedly(Return(CMAKESL_EXEC_SMOKE_TEST_ROOT_DIR +
                           std::string{ "/add_subdirectory_test/import" }));

  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(42));
}
}
