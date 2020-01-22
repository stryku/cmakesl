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
                      "    decl::product p;"
                      ""
                      "    auto all_empty = p.name == string()"
                      "        && p.name_prefix == string()"
                      "        && p.name_suffix == string()"
                      "        && is_empty(p.files)"
                      "        && is_empty(p.include_dirs)"
                      "        && is_empty(p.compile_options)"
                      "        && is_empty(p.compile_definitions)"
                      "        && is_empty(p.dependencies)"
                      "        && p.type == decl::product_type::executable;"
                      ""
                      "    return int(all_empty);"
                      "}";
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(1));
}

}
