#include "test/exec/smoke_test_fixture.hpp"

#include <gmock/gmock.h>

namespace cmsl::exec::test {
using ::testing::Eq;

using DeclarativeExecutableSmokeTest = ExecutionSmokeTest;

TEST_F(DeclarativeExecutableSmokeTest, DefaultConstructor)
{
  const auto source = "bool is_empty(decl::forwarding_lists& lists)"
                      "{"
                      "    return lists.interface.empty()"
                      "        && lists.public.empty()"
                      "        && lists.private.empty();"
                      "}"
                      ""
                      "int main()"
                      "{"
                      "    decl::executable exe;"
                      ""
                      "    auto all_empty = exe.name == string()"
                      "        && is_empty(exe.files)"
                      "        && is_empty(exe.include_dirs)"
                      "        && is_empty(exe.compile_options)"
                      "        && is_empty(exe.dependencies)"
                      "        && exe.type == decl::product_type::executable;"
                      ""
                      "    return int(all_empty);"
                      "}";
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(1));
}
}
