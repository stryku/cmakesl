#include "test/exec/smoke_test_fixture.hpp"

#include <gmock/gmock.h>

namespace cmsl::exec::test {
using ::testing::Eq;

using OptionSmokeTest = ExecutionSmokeTest;

TEST_F(OptionSmokeTest, ConstructorWithDescription)
{
  const auto source = "int main()"
                      "{"
                      "    auto foo = cmake::option(\"bar\", \"baz\");"
                      "    return int(foo.value() == false);"
                      "}";
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(1));
}

TEST_F(OptionSmokeTest, ConstructorWithDescriptionAndValue)
{
  const auto source =
    "int main()"
    "{"
    "    auto foo = cmake::option(\"bar\", \"baz\", false);"
    "    auto qux = cmake::option(\"top\", \"kek\", true);"
    "    return int(foo.value() == false && qux.value() == true);"
    "}";
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(1));
}
}
