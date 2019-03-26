#include "exec/source_executor.hpp"
#include "common/source_view.hpp"
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
                const auto result = m_executor.execute2(cmsl::source_view{ source });
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
                const auto result = m_executor.execute2(cmsl::source_view{ source });
                EXPECT_THAT(result, Eq(1));
            }

            TEST_F(StringTypeSmokeTest, ExplicitConstructorWithValue)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    string s = string(\"foo\");\n"
                        "    return int(s.size() == 3 && s.empty() == false && s == \"foo\");\n"
                        "}";
                const auto result = m_executor.execute2(cmsl::source_view{ source });
                EXPECT_THAT(result, Eq(1));
            }

            TEST_F(StringTypeSmokeTest, ExplicitConstructorWithString)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    string s = string(\"foo\");\n"
                        "    string s2 = string(s);"
                        "    return int(s2 == \"foo\");\n"
                        "}";
                const auto result = m_executor.execute2(cmsl::source_view{ source });
                EXPECT_THAT(result, Eq(1));
            }

            TEST_F(StringTypeSmokeTest, ExplicitConstructorWithRepeatedString)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    string s = string(\"foo \");\n"
                        "    string s2 = string(s, 4);"
                        "    return int(s2 == \"foo foo foo foo \");\n"
                        "}";
                const auto result = m_executor.execute2(cmsl::source_view{ source });
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
                const auto result = m_executor.execute2(cmsl::source_view{ source });
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
                const auto result = m_executor.execute2(cmsl::source_view{ source });
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
                const auto result = m_executor.execute2(cmsl::source_view{ source });
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
                const auto result = m_executor.execute2(cmsl::source_view{ source });
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
                const auto result = m_executor.execute2(cmsl::source_view{ source });
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
                const auto result = m_executor.execute2(cmsl::source_view{ source });
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
                const auto result = m_executor.execute2(cmsl::source_view{ source });
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
                const auto result = m_executor.execute2(cmsl::source_view{ source });
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
                const auto result = m_executor.execute2(cmsl::source_view{ source });
                EXPECT_THAT(result, Eq(1));
            }

            TEST_F(StringTypeSmokeTest, Clear)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    string s = \"foo +\";\n"
                        "    s.clear();\n"
                        "    return int(s.empty());\n"
                        "}";
                const auto result = m_executor.execute2(cmsl::source_view{ source });
                EXPECT_THAT(result, Eq(1));
            }

            TEST_F(StringTypeSmokeTest, Insert)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    string s;\n"
                        "    s.insert(0, \"bar\");\n"
                        "    s.insert(0, \"foo \");\n"
                        "    s.insert(7, \" baz\");\n"
                        "    return int(s == \"foo bar baz\");\n"
                        "}";
                const auto result = m_executor.execute2(cmsl::source_view{ source });
                EXPECT_THAT(result, Eq(1));
            }

            TEST_F(StringTypeSmokeTest, Erase)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    string s = \"foo bar bazrubbish\";\n"
                        "    s.erase(11);\n"
                        "    return int(s == \"foo bar baz\");\n"
                        "}";
                const auto result = m_executor.execute2(cmsl::source_view{ source });
                EXPECT_THAT(result, Eq(1));
            }

            TEST_F(StringTypeSmokeTest, ErasePos)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    int rubbish_size = \"rubbish\".size();"
                        "    string s = \"rubbishfoorubissh bar baz\";\n"
                        "    s.erase(0, rubbish_size);\n"
                        "    s.erase(3, rubbish_size);\n"
                        "    return int(s == \"foo bar baz\");\n"
                        "}";
                const auto result = m_executor.execute2(cmsl::source_view{ source });
                EXPECT_THAT(result, Eq(1));
            }

            // Todo: Introduce more test cases
            TEST_F(StringTypeSmokeTest, StartsWith)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    string s = \"foo bar baz\";\n"
                        "    return int(s.starts_with(\"foo b\"));\n"
                        "}";
                const auto result = m_executor.execute2(cmsl::source_view{ source });
                EXPECT_THAT(result, Eq(1));
            }

            // Todo: Introduce more test cases
            TEST_F(StringTypeSmokeTest, EndsWith)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    string s = \"foo bar baz\";\n"
                        "    return int(s.ends_with(\"r baz\"));\n"
                        "}";
                const auto result = m_executor.execute2(cmsl::source_view{ source });
                EXPECT_THAT(result, Eq(1));
            }

            TEST_F(StringTypeSmokeTest, Replace)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    int rubbish_size = \"rubbish\".size();"
                        "    string s = \"foo rubbish baz\";\n"
                        "    s.replace(4, rubbish_size, \"bar\");\n"
                        "    return int(s == \"foo bar baz\");\n"
                        "}";
                const auto result = m_executor.execute2(cmsl::source_view{ source });
                EXPECT_THAT(result, Eq(1));
            }

            TEST_F(StringTypeSmokeTest, Substr)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    int rubbish_size = \"rubbish \".size();"
                        "    string s = \"rubbish foo bar baz\";\n"
                        "    string s2 = s.substr(rubbish_size);\n"
                        "    return int(s2 == \"foo bar baz\");\n"
                        "}";
                const auto result = m_executor.execute2(cmsl::source_view{ source });
                EXPECT_THAT(result, Eq(1));
            }

            TEST_F(StringTypeSmokeTest, SubstrPosCount)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    int foobarbaz_size = \"foo bar baz\".size();\n"
                        "    string s = \"rub foo bar baz bish\";\n"
                        "    string s2 = s.substr(4, foobarbaz_size);\n"
                        "    return int(s2 == \"foo bar baz\");\n"
                        "}";
                const auto result = m_executor.execute2(cmsl::source_view{ source });
                EXPECT_THAT(result, Eq(1));
            }

            TEST_F(StringTypeSmokeTest, Resize)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    int foobarbaz_size = \"foo bar baz\".size();\n"
                        "    string s = \"foo bar baz rubbish\";\n"
                        "    s.resize(foobarbaz_size);\n"
                        "    return int(s == \"foo bar baz\");\n"
                        "}";
                const auto result = m_executor.execute2(cmsl::source_view{ source });
                EXPECT_THAT(result, Eq(1));
            }

            TEST_F(StringTypeSmokeTest, ResizeFill)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    string expected = \"foo bar baz qux qu\";\n"
                        "    string s = \"foo bar baz\";\n"
                        "    s.resize(expected.size(), \" qux\");\n"
                        "    return int(s == expected);\n"
                        "}";
                const auto result = m_executor.execute2(cmsl::source_view{ source });
                EXPECT_THAT(result, Eq(1));
            }

            // Todo: Add more test cases
            TEST_F(StringTypeSmokeTest, Find)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    string s = \"foo bar baz\";\n"
                        "    int found_at = s.find(\"bar\");\n"
                        "    return int(found_at == 4);\n"
                        "}";
                const auto result = m_executor.execute2(cmsl::source_view{ source });
                EXPECT_THAT(result, Eq(1));
            }

            // Todo: Add more test cases
            TEST_F(StringTypeSmokeTest, FindFromPos)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    string s = \"foo bar baz bar\";\n"
                        "    int found_at = s.find(\"bar\", 6);\n"
                        "    return int(found_at == 12);\n"
                        "}";
                const auto result = m_executor.execute2(cmsl::source_view{ source });
                EXPECT_THAT(result, Eq(1));
            }

            // Todo: Add more test cases
            TEST_F(StringTypeSmokeTest, FindNotOf)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    string s = \"baz bar foo\";\n"
                        "    int found_at = s.find_not_of(\"abrz \");\n"
                        "    return int(found_at == 8);\n"
                        "}";
                const auto result = m_executor.execute2(cmsl::source_view{ source });
                EXPECT_THAT(result, Eq(1));
            }

            // Todo: Add more test cases
            TEST_F(StringTypeSmokeTest, FindNotOfFromPos)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    string s = \"foo baz bar foo\";\n"
                        "    int found_at = s.find_not_of(\"abrz \", 3);\n"
                        "    return int(found_at == 12);\n"
                        "}";
                const auto result = m_executor.execute2(cmsl::source_view{ source });
                EXPECT_THAT(result, Eq(1));
            }

            // Todo: Add more test cases
            TEST_F(StringTypeSmokeTest, FindLast)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    string s = \"foo baz bar foo\";\n"
                        "    int found_at = s.find_last(\"foo\");\n"
                        "    return int(found_at == 12);\n"
                        "}";
                const auto result = m_executor.execute2(cmsl::source_view{ source });
                EXPECT_THAT(result, Eq(1));
            }

            // Todo: Add more test cases
            TEST_F(StringTypeSmokeTest, FindLastNotOf)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    string s = \"foo baz bar foo\";\n"
                        "    int found_at = s.find_last_not_of(\"fo \");\n"
                        "    return int(found_at == 10);\n"
                        "}";
                const auto result = m_executor.execute2(cmsl::source_view{ source });
                EXPECT_THAT(result, Eq(1));
            }

            // Todo: Add more test cases
            TEST_F(StringTypeSmokeTest, Contains)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    string s = \"foo baz bar\";\n"
                        "    return int(s.contains(\"z b\"));\n"
                        "}";
                const auto result = m_executor.execute2(cmsl::source_view{ source });
                EXPECT_THAT(result, Eq(1));
            }

            // Todo: Add more test cases
            TEST_F(StringTypeSmokeTest, Lower)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    string s = \"qwertyuiopasdfghjklzxcvbnm01234567890QWERTYUIOPASDFGHJKLZXCVBNM\";\n"
                        "    s.lower();"
                        "    return int(s == \"qwertyuiopasdfghjklzxcvbnm01234567890qwertyuiopasdfghjklzxcvbnm\");\n"
                        "}";
                const auto result = m_executor.execute2(cmsl::source_view{ source });
                EXPECT_THAT(result, Eq(1));
            }

            // Todo: Add more test cases
            TEST_F(StringTypeSmokeTest, MakeLower)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    string s = \"qwertyuiopasdfghjklzxcvbnm01234567890QWERTYUIOPASDFGHJKLZXCVBNM\";\n"
                        "    string s2 = s.make_lower();"
                        "    bool test1 = s == \"qwertyuiopasdfghjklzxcvbnm01234567890QWERTYUIOPASDFGHJKLZXCVBNM\";\n"
                        "    bool test2 = s2 == \"qwertyuiopasdfghjklzxcvbnm01234567890qwertyuiopasdfghjklzxcvbnm\";\n"
                        "    return int(test1 && test2);\n"
                        "}";
                const auto result = m_executor.execute2(cmsl::source_view{ source });
                EXPECT_THAT(result, Eq(1));
            }

            // Todo: Add more test cases
            TEST_F(StringTypeSmokeTest, Upper)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    string s = \"qwertyuiopasdfghjklzxcvbnm01234567890QWERTYUIOPASDFGHJKLZXCVBNM\";\n"
                        "    s.upper();"
                        "    return int(s == \"QWERTYUIOPASDFGHJKLZXCVBNM01234567890QWERTYUIOPASDFGHJKLZXCVBNM\");\n"
                        "}";
                const auto result = m_executor.execute2(cmsl::source_view{ source });
                EXPECT_THAT(result, Eq(1));
            }

            // Todo: Add more test cases
            TEST_F(StringTypeSmokeTest, MakeUpper)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    string s = \"qwertyuiopasdfghjklzxcvbnm01234567890QWERTYUIOPASDFGHJKLZXCVBNM\";\n"
                        "    string s2 = s.make_upper();"
                        "    bool test1 = s == \"qwertyuiopasdfghjklzxcvbnm01234567890QWERTYUIOPASDFGHJKLZXCVBNM\";\n"
                        "    bool test2 = s2 == \"QWERTYUIOPASDFGHJKLZXCVBNM01234567890QWERTYUIOPASDFGHJKLZXCVBNM\";\n"
                        "    return int(test1 && test2);\n"
                        "}";
                const auto result = m_executor.execute2(cmsl::source_view{ source });
                EXPECT_THAT(result, Eq(1));
            }
        }
    }
}
