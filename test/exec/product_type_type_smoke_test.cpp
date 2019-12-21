#include "test/exec/smoke_test_fixture.hpp"

#include <gmock/gmock.h>

namespace cmsl::exec::test {
using ::testing::Eq;

using ProductTypeTypeSmokeTest = ExecutionSmokeTest;

TEST_F(ProductTypeTypeSmokeTest, DefaultConstructor)
{
  const auto source = "int main()"
                      "{"
                      "    decl::product_type pt;"
                      "    auto result = pt == decl::product_type::executable;"
                      "    return int(result);"
                      "}";
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(1));
}
}
