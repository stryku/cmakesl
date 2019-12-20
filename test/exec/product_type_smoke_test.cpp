#include "test/exec/smoke_test_fixture.hpp"

#include <gmock/gmock.h>

namespace cmsl::exec::test {
using ::testing::Eq;

using ProductTypeSmokeTest = ExecutionSmokeTest;

TEST_F(ProductTypeSmokeTest, DefaultConstructor)
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
                      "    decl::forwarding_lists empty_forward_lists;"
                      "    decl::product p;"
                      ""
                      "    auto all_empty = p.name == string()"
                      "        && is_empty(p.files)"
                      "        && is_empty(p.include_dirs)"
                      "        && is_empty(p.compile_options)"
                      "        && is_empty(p.dependencies);"
                      ""
                      "    return int(all_empty);"
                      "}";
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(1));
}

}
