#include "test/exec/smoke_test_fixture.hpp"

#include <gmock/gmock.h>

namespace cmsl::exec::test {
using ::testing::Eq;

using DeclarativeStaticLibrarySmokeTest = ExecutionSmokeTest;

TEST_F(DeclarativeStaticLibrarySmokeTest, DefaultConstructor)
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
                      "    decl::static_library lib;"
                      ""
                      "    auto all_empty = lib.name == string()"
                      "        && is_empty(lib.files)"
                      "        && is_empty(lib.include_dirs)"
                      "        && is_empty(lib.compile_options)"
                      "        && is_empty(lib.dependencies)"
                      "        && lib.type == decl::product_type::executable;"
                      ""
                      "    return int(all_empty);"
                      "}";
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(1));
}
}
