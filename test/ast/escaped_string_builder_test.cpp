#include "ast/escaped_string_builder.hpp"

#include <gmock/gmock.h>

namespace cmsl::ast::test {
using ::testing::Eq;

TEST(EscapedStringBuilderTest, EscapesCorrectly)
{
  cmsl::string_view to_escape = "\\\\foo\\tbar\\nbaz";
  const auto result = escaped_string_builder::escape(to_escape);
  EXPECT_THAT(result, Eq("\\foo\tbar\nbaz"));
}
}
