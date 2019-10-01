#include "test/exec/smoke_test_fixture.hpp"

#include <gmock/gmock.h>

namespace cmsl::exec::test {
using ::testing::Eq;
using ::testing::Return;

using FatalErrorSmokeTest = ExecutionSmokeTest;

TEST_F(FatalErrorSmokeTest, StandaloneCall_MainReturnsMinusOne)
{
  const auto source = "int main()"
                      "{"
                      "    cmake::fatal_error(\"msg\");"
                      "}";

  EXPECT_CALL(m_facade, did_fatal_error_occure()).WillRepeatedly(Return(true));

  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(-1));
}

TEST_F(FatalErrorSmokeTest, ErroInFunction_MainReturnsMinusOne)
{
  const auto source = "void raises_error()"
                      "{"
                      "    cmake::fatal_error(\"bar\");"
                      "}"
                      ""
                      "int main()"
                      "{"
                      "    raises_error();"
                      "}";

  EXPECT_CALL(m_facade, did_fatal_error_occure()).WillRepeatedly(Return(true));

  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(-1));
}

TEST_F(FatalErrorSmokeTest, ErroInBinaryOperator_MainReturnsMinusOne)
{
  const auto source = "int raises_error()"
                      "{"
                      "    cmake::fatal_error(\"bar\");"
                      "}"
                      ""
                      "int main()"
                      "{"
                      "    raises_error() + 42;"
                      "}";

  EXPECT_CALL(m_facade, did_fatal_error_occure()).WillRepeatedly(Return(true));

  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(-1));
}

TEST_F(FatalErrorSmokeTest, ErroInFunctionCallParameter_MainReturnsMinusOne)
{
  const auto source = "int raises_error()"
                      "{"
                      "    cmake::fatal_error(\"bar\");"
                      "}"
                      ""
                      "void foo(int value)"
                      "{}"
                      ""
                      "int main()"
                      "{"
                      "    foo(raises_error());"
                      "}";

  EXPECT_CALL(m_facade, did_fatal_error_occure()).WillRepeatedly(Return(true));

  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(-1));
}

TEST_F(FatalErrorSmokeTest,
       ErroInSecondFunctionCallParameter_MainReturnsMinusOne)
{
  const auto source = "int raises_error()"
                      "{"
                      "    cmake::fatal_error(\"bar\");"
                      "}"
                      ""
                      "void foo(int val1, int val2)"
                      "{}"
                      ""
                      "int main()"
                      "{"
                      "    foo(42, raises_error());"
                      "}";

  EXPECT_CALL(m_facade, did_fatal_error_occure())
    .WillOnce(Return(false))
    .WillRepeatedly(Return(true));

  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(-1));
}

TEST_F(FatalErrorSmokeTest, ErroInLhsOfMemberFunctionCall_MainReturnsMinusOne)
{
  const auto source = "int raises_error()"
                      "{"
                      "    cmake::fatal_error(\"bar\");"
                      "}"
                      ""
                      "int main()"
                      "{"
                      "    raises_error().to_string();"
                      "}";

  EXPECT_CALL(m_facade, did_fatal_error_occure()).WillRepeatedly(Return(true));

  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(-1));
}

TEST_F(FatalErrorSmokeTest, ErroInLhsOfClassMemberAccess_MainReturnsMinusOne)
{
  const auto source = "class foo"
                      "{"
                      "    int bar;"
                      "};"
                      ""
                      "foo raises_error()"
                      "{"
                      "    cmake::fatal_error(\"bar\");"
                      "}"
                      ""
                      "int main()"
                      "{"
                      "    raises_error().bar;"
                      "}";

  EXPECT_CALL(m_facade, did_fatal_error_occure()).WillRepeatedly(Return(true));

  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(-1));
}

TEST_F(FatalErrorSmokeTest, ErroInInitializerList_MainReturnsMinusOne)
{
  const auto source = "int raises_error()"
                      "{"
                      "    cmake::fatal_error(\"bar\");"
                      "}"
                      ""
                      "int main()"
                      "{"
                      "    auto l = { 41, raises_error(), 42 };"
                      "}";

  EXPECT_CALL(m_facade, did_fatal_error_occure())
    .WillOnce(Return(false))
    .WillRepeatedly(Return(true));

  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(-1));
}

TEST_F(FatalErrorSmokeTest, ErroInTernaryOperatorCondition_MainReturnsMinusOne)
{
  const auto source = "int raises_error()"
                      "{"
                      "    cmake::fatal_error(\"bar\");"
                      "}"
                      ""
                      "int main()"
                      "{"
                      "    raises_error() == 0 ? true : false;"
                      "}";

  EXPECT_CALL(m_facade, did_fatal_error_occure()).WillRepeatedly(Return(true));

  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(-1));
}

TEST_F(FatalErrorSmokeTest, ErroInDesignatedInitializers_MainReturnsMinusOne)
{
  const auto source = "class foo"
                      "{"
                      "    int bar;"
                      "};"
                      ""
                      "int raises_error()"
                      "{"
                      "    cmake::fatal_error(\"bar\");"
                      "}"
                      ""
                      "int main()"
                      "{"
                      "    foo f = { .bar = raises_error() };"
                      "}";

  EXPECT_CALL(m_facade, did_fatal_error_occure()).WillRepeatedly(Return(true));

  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(-1));
}
}
