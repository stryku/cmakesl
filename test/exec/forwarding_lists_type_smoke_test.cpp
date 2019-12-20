#include "test/exec/smoke_test_fixture.hpp"

#include <gmock/gmock.h>

namespace cmsl::exec::test {
using ::testing::Eq;

using ForwardingListsTypeSmokeTest = ExecutionSmokeTest;

TEST_F(ForwardingListsTypeSmokeTest, DefaultConstruction)
{
  const auto source = "int main()"
                      "{"
                      "    decl::forwarding_lists lists;"
                      ""
                      "    auto all_empty = lists.private.empty()"
                      "        && lists.public.empty()"
                      "        && lists.interface.empty();"
                      ""
                      "    return int(all_empty);"
                      "}";
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(1));
}
}
