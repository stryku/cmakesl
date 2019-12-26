#include "test/exec/smoke_test_fixture.hpp"

#include <gmock/gmock.h>

namespace cmsl::exec::test {
using ::testing::Eq;

using ObjectSlicingSmokeTest = ExecutionSmokeTest;

TEST_F(ObjectSlicingSmokeTest, CastToBaseValue)
{
  const auto source = "int main()"
                      "{"
                      "    decl::executable exe;"
                      "    exe.files.public += \"foo\";"
                      ""
                      "    decl::product product = exe;"
                      ""
                      "    return int(product.files.public.at(0) == \"foo\");"
                      "}";
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(1));
}

TEST_F(ObjectSlicingSmokeTest, CastToReferenceToBase)
{
  const auto source = "int main()"
                      "{"
                      "    decl::executable exe;"
                      "    decl::product& product = exe;"
                      ""
                      "    product.files.public += \"foo\";"
                      ""
                      "    return int(exe.files.public.at(0) == \"foo\");"
                      "}";
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(1));
}
}
