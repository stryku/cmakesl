#include "sema/sema_type.hpp"
#include "test/exec/smoke_test_fixture.hpp"

namespace cmsl::exec::test {
using ::testing::Eq;
using VersionTypeSmokeTest = ExecutionSmokeTest;

TEST_F(VersionTypeSmokeTest, ConstructorMajor)
{
  const auto source = "int main()"
                      "{"
                      "    version v = version(1);"
                      "    return int(v.major() == 1 && v.minor() == 0 && "
                      "v.patch() == 0 && v.tweak() == 0);"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(1));
}

TEST_F(VersionTypeSmokeTest, ConstructorMajorMinor)
{
  const auto source = "int main()"
                      "{"
                      "    version v = version(1, 2);"
                      "    return int(v.major() == 1 && v.minor() == 2 && "
                      "v.patch() == 0 && v.tweak() == 0);"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(1));
}

TEST_F(VersionTypeSmokeTest, ConstructorMajorMinorPatch)
{
  const auto source = "int main()"
                      "{"
                      "    version v = version(1, 2, 3);"
                      "    return int(v.major() == 1 && v.minor() == 2 && "
                      "v.patch() == 3 && v.tweak() == 0);"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(1));
}

TEST_F(VersionTypeSmokeTest, ConstructorMajorMinorPatchTweak)
{
  const auto source = "int main()"
                      "{"
                      "    version v = version(1, 2, 3, 4);"
                      "    return int(v.major() == 1 && v.minor() == 2 && "
                      "v.patch() == 3 && v.tweak() == 4);"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(1));
}

TEST_F(VersionTypeSmokeTest, OperatorEqualEqualAndNotEqual)
{
  const auto source = "int main()"
                      "{"
                      "    version v = version(1, 2, 3, 4);"
                      "    version v2 = version(1, 2, 3, 4);"
                      "    version v3 = version(0, 2, 3, 4);"
                      "    version v4 = version(1, 0, 3, 4);"
                      "    version v5 = version(1, 2, 0, 4);"
                      "    version v6 = version(1, 2, 3, 0);"
                      "    return int(v == v2 &&"
                      "               v != v3 &&"
                      "               v != v4 &&"
                      "               v != v5 &&"
                      "               v != v6);"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(1));
}

TEST_F(VersionTypeSmokeTest, OperatorLess)
{
  const auto source =
    "int main()"
    "{"
    "    version v = version(1, 9, 9, 9);"
    "    version v2 = version(1, 2, 9, 9);"
    "    version v3 = version(1, 2, 3, 9);"
    "    version v4 = version(1, 2, 3, 4);"
    ""
    "    version v5 = version(2, 0, 0, 0);"
    "    version v6 = version(1, 3, 0, 0);"
    "    version v7 = version(1, 2, 4, 0);"
    "    version v8 = version(1, 2, 3, 5);"
    "    return int(v < v5 && v2 < v6 && v3 < v7 && v4 < v8);"
    "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(1));
}

TEST_F(VersionTypeSmokeTest, OperatorLessEqual)
{
  const auto source = "int main()"
                      "{"
                      "    version v = version(1, 9, 9, 9);"
                      "    version v2 = version(1, 2, 9, 9);"
                      "    version v3 = version(1, 2, 3, 9);"
                      "    version v4 = version(1, 2, 3, 4);"
                      ""
                      "    version v5 = version(2, 0, 0, 0);"
                      "    version v6 = version(1, 3, 0, 0);"
                      "    version v7 = version(1, 2, 4, 0);"
                      "    version v8 = version(1, 2, 3, 5);"
                      ""
                      "    version v9 = version(1, 2, 3, 4);"
                      "    version v10 = version(1, 2, 3, 4);"
                      "    return int(v <= v5 && v2 <= v6 && v3 <= v7 && v4 "
                      "<= v8 && v9 <= v10);"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(1));
}

TEST_F(VersionTypeSmokeTest, OperatorGreater)
{
  const auto source =
    "int main()"
    "{"
    "    version v = version(1, 9, 9, 9);"
    "    version v2 = version(1, 2, 9, 9);"
    "    version v3 = version(1, 2, 3, 9);"
    "    version v4 = version(1, 2, 3, 4);"
    ""
    "    version v5 = version(2, 0, 0, 0);"
    "    version v6 = version(1, 3, 0, 0);"
    "    version v7 = version(1, 2, 4, 0);"
    "    version v8 = version(1, 2, 3, 5);"
    "    return int(v5 > v && v6 > v2 && v7 > v3 && v8 > v4);"
    "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(1));
}

TEST_F(VersionTypeSmokeTest, OperatorGreaterEqual)
{
  const auto source = "int main()"
                      "{"
                      "    version v = version(1, 9, 9, 9);"
                      "    version v2 = version(1, 2, 9, 9);"
                      "    version v3 = version(1, 2, 3, 9);"
                      "    version v4 = version(1, 2, 3, 4);"
                      ""
                      "    version v5 = version(2, 0, 0, 0);"
                      "    version v6 = version(1, 3, 0, 0);"
                      "    version v7 = version(1, 2, 4, 0);"
                      "    version v8 = version(1, 2, 3, 5);"
                      ""
                      "    version v9 = version(1, 2, 3, 4);"
                      "    version v10 = version(1, 2, 3, 4);"
                      "    return int(v5 >= v && v6 >= v2 && v7 >= v3 && v8 "
                      ">= v4 && v10 >= v9);"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(1));
}

TEST_F(VersionTypeSmokeTest, ToString)
{
  const auto source = "int main()"
                      "{"
                      "    version v = version(1, 2, 3, 4);"
                      "    return int(v.to_string() == \"1.2.3.4\");"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(1));
}
}
