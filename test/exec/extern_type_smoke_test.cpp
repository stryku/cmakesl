#include "test/exec/smoke_test_fixture.hpp"

#include <gmock/gmock.h>

namespace cmsl::exec::test {
using ::testing::Eq;
using ::testing::Return;
using ::testing::DoubleNear;

using ExternTypeSmokeTest = ExecutionSmokeTest;

TEST_F(ExternTypeSmokeTest, Creation)
{
  const auto source = "int main()"
                      "{"
                      "    auto foo = extern<int>(\"bar\");"
                      "    return 42;"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(ExternTypeSmokeTest, NotExisting_HasValue_ReturnsFalse)
{
  const auto source = "int main()"
                      "{"
                      "    auto foo = extern<int>(\"bar\");"
                      "    return int(foo.has_value());"
                      "}";

  EXPECT_CALL(m_facade, try_get_extern_define("bar"))
    .WillOnce(Return(std::nullopt));

  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(0));
}

TEST_F(ExternTypeSmokeTest, Existing_HasValue_ReturnsTrue)
{
  const auto source = "int main()"
                      "{"
                      "    auto foo = extern<int>(\"bar\");"
                      "    return int(foo.has_value());"
                      "}";

  EXPECT_CALL(m_facade, try_get_extern_define("bar")).WillOnce(Return("42"));

  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(1));
}

TEST_F(ExternTypeSmokeTest, ExistingBool_Value_ReturnsCorrectlyParsedBoolValue)
{
  const auto source =
    "int main()"
    "{"
    "    auto foo = extern<bool>(\"bar\");"
    "    auto baz = extern<bool>(\"qux\");"
    "    return int(foo.value() == false && baz.value() == true);"
    "}";

  EXPECT_CALL(m_facade, try_get_extern_define("bar"))
    .WillOnce(Return("false"));
  EXPECT_CALL(m_facade, try_get_extern_define("qux")).WillOnce(Return("true"));

  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(1));
}

TEST_F(ExternTypeSmokeTest, ExistingInt_Value_ReturnsCorrectlyParsedIntValue)
{
  const auto source = "int main()"
                      "{"
                      "    auto foo = extern<int>(\"bar\");"
                      "    return foo.value();"
                      "}";

  EXPECT_CALL(m_facade, try_get_extern_define("bar")).WillOnce(Return("42"));

  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(ExternTypeSmokeTest, ExistingDouble_Value_ReturnsCorrectlyParsedDouble)
{
  const auto source = "int main()"
                      "{"
                      "    auto foo = extern<double>(\"bar\");"
                      "    return int(foo.value());"
                      "}";

  EXPECT_CALL(m_facade, try_get_extern_define("bar")).WillOnce(Return("42.0"));

  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(ExternTypeSmokeTest, ExistingString_Value_ReturnsCorrectlyParsedString)
{
  const auto source = "int main()"
                      "{"
                      "    auto foo = extern<string>(\"bar\");"
                      "    return int(foo.value() == \"42\");"
                      "}";

  EXPECT_CALL(m_facade, try_get_extern_define("bar")).WillOnce(Return("42"));

  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(1));
}
}
