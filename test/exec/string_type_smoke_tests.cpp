#include "exec/source_executor.hpp"
#include "test/exec/mock/cmake_facade_mock.hpp"

#include <gmock/gmock.h>

namespace cmsl
{
    namespace exec
    {
        namespace test
        {
            using ::testing::Eq;

            class StringTypeSmokeTest : public ::testing::Test
            {
            protected:
                cmake_facade_mock m_facade;
                source_executor m_executor{m_facade};
            };

            TEST_F(StringTypeSmokeTest, DefaultConstructor)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    string s;\n"
                        "    return int(s.size() == 0 && s.empty());\n"
                        "}";
                const auto result = m_executor.execute2(source);
                EXPECT_THAT(result, Eq(1));
            }

            TEST_F(StringTypeSmokeTest, ExplicitDefaultConstructor)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    string s = string();\n"
                        "    return int(s.size() == 0 && s.empty());\n"
                        "}";
                const auto result = m_executor.execute2(source);
                EXPECT_THAT(result, Eq(1));
            }

            TEST_F(StringTypeSmokeTest, ExplicitConstructorWithValue)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    string s = string(\"foo\");\n"
                        "    return int(s.size() == 3 && s.empty() == false);\n"
                        "}";
                const auto result = m_executor.execute2(source);
                EXPECT_THAT(result, Eq(1));
            }

            TEST_F(StringTypeSmokeTest, ExplicitConstructorWithString)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    string s = string(\"foo\");\n"
                        "    string s2 = string(s);"
                        "    return int(s2.size() == 3 && s2.empty() == false);\n"
                        "}";
                const auto result = m_executor.execute2(source);
                EXPECT_THAT(result, Eq(1));
            }

            TEST_F(StringTypeSmokeTest, OperatorEqual)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    string s = \"foo\";\n"
                        "    string s2 = s;"
                        "    return int(s2.size() == 3 && s2.empty() == false);\n"
                        "}";
                const auto result = m_executor.execute2(source);
                EXPECT_THAT(result, Eq(1));
            }

            TEST_F(StringTypeSmokeTest, OperatorEqualEqual)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    string s = \"foo\";\n"
                        "    string s2 = s;"
                        "    return int(s == s2);\n"
                        "}";
                const auto result = m_executor.execute2(source);
                EXPECT_THAT(result, Eq(1));
            }

            TEST_F(StringTypeSmokeTest, OperatorNotEqual)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    string s = \"foo\";\n"
                        "    string s2 = \"fooo\";\n"
                        "    return int(s != s2);\n"
                        "}";
                const auto result = m_executor.execute2(source);
                EXPECT_THAT(result, Eq(1));
            }

            TEST_F(StringTypeSmokeTest, OperatorLess)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    string s = \"bar\";\n"
                        "    string s2 = \"foo\";\n"
                        "    return int(s < s2);\n"
                        "}";
                const auto result = m_executor.execute2(source);
                EXPECT_THAT(result, Eq(1));
            }

            TEST_F(StringTypeSmokeTest, OperatorLessEqual)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    string s = \"bar\";\n"
                        "    string s2 = \"bar\";\n"
                        "    return int(s <= s2);\n"
                        "}";
                const auto result = m_executor.execute2(source);
                EXPECT_THAT(result, Eq(1));
            }

            TEST_F(StringTypeSmokeTest, OperatorGreater)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    string s = \"foo\";\n"
                        "    string s2 = \"bar\";\n"
                        "    return int(s > s2);\n"
                        "}";
                const auto result = m_executor.execute2(source);
                EXPECT_THAT(result, Eq(1));
            }

            TEST_F(StringTypeSmokeTest, OperatorGreaterEqual)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    string s = \"bar\";\n"
                        "    string s2 = \"bar\";\n"
                        "    return int(s >= s2);\n"
                        "}";
                const auto result = m_executor.execute2(source);
                EXPECT_THAT(result, Eq(1));
            }

            TEST_F(StringTypeSmokeTest, OperatorPlus)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    string s = \"foo +\";\n"
                        "    string s2 = \" bar\";\n"
                        "    string s3 = s + s2;\n"
                        "    return int(s3 == \"foo + bar\");\n"
                        "}";
                const auto result = m_executor.execute2(source);
                EXPECT_THAT(result, Eq(1));
            }

            TEST_F(StringTypeSmokeTest, OperatorPlusEqual)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    string s = \"foo +\";\n"
                        "    s += \" bar\";\n"
                        "    return int(s == \"foo + bar\");\n"
                        "}";
                const auto result = m_executor.execute2(source);
                EXPECT_THAT(result, Eq(1));
            }
        }
    }
}
