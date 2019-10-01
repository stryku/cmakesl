#include "test/exec/smoke_test_fixture.hpp"

#include <gmock/gmock.h>

namespace cmsl::exec::test {
using ::testing::Eq;
using StringTypeSmokeTest = ExecutionSmokeTest;

TEST_F(StringTypeSmokeTest, DefaultConstructor)
{
  const auto source = "int main()"
                      "{"
                      "    string s;"
                      "    return int(s.size() == 0 && s.empty());"
                      "}";
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(1));
}

TEST_F(StringTypeSmokeTest, ExplicitDefaultConstructor)
{
  const auto source = "int main()"
                      "{"
                      "    string s = string();"
                      "    return int(s.size() == 0 && s.empty());"
                      "}";
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(1));
}

TEST_F(StringTypeSmokeTest, ExplicitConstructorWithValue)
{
  const auto source =
    "int main()"
    "{"
    "    string s = string(\"foo\");"
    "    return int(s.size() == 3 && s.empty() == false && s == \"foo\");"
    "}";
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(1));
}

TEST_F(StringTypeSmokeTest, ExplicitConstructorWithString)
{
  const auto source = "int main()"
                      "{"
                      "    string s = string(\"foo\");"
                      "    string s2 = string(s);"
                      "    return int(s2 == \"foo\");"
                      "}";
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(1));
}

TEST_F(StringTypeSmokeTest, ExplicitConstructorWithRepeatedString)
{
  const auto source = "int main()"
                      "{"
                      "    string s = string(\"foo \");"
                      "    string s2 = string(s, 4);"
                      "    return int(s2 == \"foo foo foo foo \");"
                      "}";
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(1));
}

TEST_F(StringTypeSmokeTest, OperatorEqual)
{
  const auto source = "int main()"
                      "{"
                      "    string s = \"foo\";"
                      "    string s2 = s;"
                      "    return int(s2.size() == 3 && s2.empty() == false);"
                      "}";
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(1));
}

TEST_F(StringTypeSmokeTest, OperatorEqualEqual)
{
  const auto source = "int main()"
                      "{"
                      "    string s = \"foo\";"
                      "    string s2 = s;"
                      "    return int(s == s2);"
                      "}";
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(1));
}

TEST_F(StringTypeSmokeTest, OperatorNotEqual)
{
  const auto source = "int main()"
                      "{"
                      "    string s = \"foo\";"
                      "    string s2 = \"fooo\";"
                      "    return int(s != s2);"
                      "}";
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(1));
}

TEST_F(StringTypeSmokeTest, OperatorLess)
{
  const auto source = "int main()"
                      "{"
                      "    string s = \"bar\";"
                      "    string s2 = \"foo\";"
                      "    return int(s < s2);"
                      "}";
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(1));
}

TEST_F(StringTypeSmokeTest, OperatorLessEqual)
{
  const auto source = "int main()"
                      "{"
                      "    string s = \"bar\";"
                      "    string s2 = \"bar\";"
                      "    return int(s <= s2);"
                      "}";
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(1));
}

TEST_F(StringTypeSmokeTest, OperatorGreater)
{
  const auto source = "int main()"
                      "{"
                      "    string s = \"foo\";"
                      "    string s2 = \"bar\";"
                      "    return int(s > s2);"
                      "}";
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(1));
}

TEST_F(StringTypeSmokeTest, OperatorGreaterEqual)
{
  const auto source = "int main()"
                      "{"
                      "    string s = \"bar\";"
                      "    string s2 = \"bar\";"
                      "    return int(s >= s2);"
                      "}";
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(1));
}

TEST_F(StringTypeSmokeTest, OperatorPlus)
{
  const auto source = "int main()"
                      "{"
                      "    string s = \"foo +\";"
                      "    string s2 = \" bar\";"
                      "    string s3 = s + s2;"
                      "    return int(s3 == \"foo + bar\");"
                      "}";
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(1));
}

TEST_F(StringTypeSmokeTest, OperatorPlusEqual)
{
  const auto source = "int main()"
                      "{"
                      "    string s = \"foo +\";"
                      "    s += \" bar\";"
                      "    return int(s == \"foo + bar\");"
                      "}";
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(1));
}

TEST_F(StringTypeSmokeTest, Clear)
{
  const auto source = "int main()"
                      "{"
                      "    string s = \"foo +\";"
                      "    s.clear();"
                      "    return int(s.empty());"
                      "}";
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(1));
}

TEST_F(StringTypeSmokeTest, Insert)
{
  const auto source = "int main()"
                      "{"
                      "    string s;"
                      "    s.insert(0, \"bar\");"
                      "    s.insert(0, \"foo \");"
                      "    s.insert(7, \" baz\");"
                      "    return int(s == \"foo bar baz\");"
                      "}";
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(1));
}

TEST_F(StringTypeSmokeTest, Erase)
{
  const auto source = "int main()"
                      "{"
                      "    string s = \"foo bar bazrubbish\";"
                      "    s.erase(11);"
                      "    return int(s == \"foo bar baz\");"
                      "}";
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(1));
}

TEST_F(StringTypeSmokeTest, ErasePos)
{
  const auto source = "int main()"
                      "{"
                      "    int rubbish_size = \"rubbish\".size();"
                      "    string s = \"rubbishfoorubissh bar baz\";"
                      "    s.erase(0, rubbish_size);"
                      "    s.erase(3, rubbish_size);"
                      "    return int(s == \"foo bar baz\");"
                      "}";
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(1));
}

// Todo: Introduce more test cases
TEST_F(StringTypeSmokeTest, StartsWith)
{
  const auto source = "int main()"
                      "{"
                      "    string s = \"foo bar baz\";"
                      "    return int(s.starts_with(\"foo b\"));"
                      "}";
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(1));
}

// Todo: Introduce more test cases
TEST_F(StringTypeSmokeTest, EndsWith)
{
  const auto source = "int main()"
                      "{"
                      "    string s = \"foo bar baz\";"
                      "    return int(s.ends_with(\"r baz\"));"
                      "}";
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(1));
}

TEST_F(StringTypeSmokeTest, Replace)
{
  const auto source = "int main()"
                      "{"
                      "    int rubbish_size = \"rubbish\".size();"
                      "    string s = \"foo rubbish baz\";"
                      "    s.replace(4, rubbish_size, \"bar\");"
                      "    return int(s == \"foo bar baz\");"
                      "}";
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(1));
}

TEST_F(StringTypeSmokeTest, Substr)
{
  const auto source = "int main()"
                      "{"
                      "    int rubbish_size = \"rubbish \".size();"
                      "    string s = \"rubbish foo bar baz\";"
                      "    string s2 = s.substr(rubbish_size);"
                      "    return int(s2 == \"foo bar baz\");"
                      "}";
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(1));
}

TEST_F(StringTypeSmokeTest, SubstrPosCount)
{
  const auto source = "int main()"
                      "{"
                      "    int foobarbaz_size = \"foo bar baz\".size();"
                      "    string s = \"rub foo bar baz bish\";"
                      "    string s2 = s.substr(4, foobarbaz_size);"
                      "    return int(s2 == \"foo bar baz\");"
                      "}";
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(1));
}

TEST_F(StringTypeSmokeTest, Resize)
{
  const auto source = "int main()"
                      "{"
                      "    int foobarbaz_size = \"foo bar baz\".size();"
                      "    string s = \"foo bar baz rubbish\";"
                      "    s.resize(foobarbaz_size);"
                      "    return int(s == \"foo bar baz\");"
                      "}";
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(1));
}

TEST_F(StringTypeSmokeTest, ResizeFill)
{
  const auto source = "int main()"
                      "{"
                      "    string expected = \"foo bar baz qux qu\";"
                      "    string s = \"foo bar baz\";"
                      "    s.resize(expected.size(), \" qux\");"
                      "    return int(s == expected);"
                      "}";
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(1));
}

// Todo: Add more test cases
TEST_F(StringTypeSmokeTest, Find)
{
  const auto source = "int main()"
                      "{"
                      "    string s = \"foo bar baz\";"
                      "    int found_at = s.find(\"bar\");"
                      "    return int(found_at == 4);"
                      "}";
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(1));
}

// Todo: Add more test cases
TEST_F(StringTypeSmokeTest, FindFromPos)
{
  const auto source = "int main()"
                      "{"
                      "    string s = \"foo bar baz bar\";"
                      "    int found_at = s.find(\"bar\", 6);"
                      "    return int(found_at == 12);"
                      "}";
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(1));
}

// Todo: Add more test cases
TEST_F(StringTypeSmokeTest, FindNotOf)
{
  const auto source = "int main()"
                      "{"
                      "    string s = \"baz bar foo\";"
                      "    int found_at = s.find_not_of(\"abrz \");"
                      "    return int(found_at == 8);"
                      "}";
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(1));
}

// Todo: Add more test cases
TEST_F(StringTypeSmokeTest, FindNotOfFromPos)
{
  const auto source = "int main()"
                      "{"
                      "    string s = \"foo baz bar foo\";"
                      "    int found_at = s.find_not_of(\"abrz \", 3);"
                      "    return int(found_at == 12);"
                      "}";
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(1));
}

// Todo: Add more test cases
TEST_F(StringTypeSmokeTest, FindLast)
{
  const auto source = "int main()"
                      "{"
                      "    string s = \"foo baz bar foo\";"
                      "    int found_at = s.find_last(\"foo\");"
                      "    return int(found_at == 12);"
                      "}";
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(1));
}

// Todo: Add more test cases
TEST_F(StringTypeSmokeTest, FindLastNotOf)
{
  const auto source = "int main()"
                      "{"
                      "    string s = \"foo baz bar foo\";"
                      "    int found_at = s.find_last_not_of(\"fo \");"
                      "    return int(found_at == 10);"
                      "}";
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(1));
}

// Todo: Add more test cases
TEST_F(StringTypeSmokeTest, Contains)
{
  const auto source = "int main()"
                      "{"
                      "    string s = \"foo baz bar\";"
                      "    return int(s.contains(\"z b\"));"
                      "}";
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(1));
}

// Todo: Add more test cases
TEST_F(StringTypeSmokeTest, Lower)
{
  const auto source =
    "int main()"
    "{"
    "    string s = "
    "\"qwertyuiopasdfghjklzxcvbnm01234567890QWERTYUIOPASDFGHJKLZXCVBNM\";"
    "    s.lower();"
    "    return int(s == "
    "\"qwertyuiopasdfghjklzxcvbnm01234567890qwertyuiopasdfghjklzxcvbnm\");"
    "}";
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(1));
}

// Todo: Add more test cases
TEST_F(StringTypeSmokeTest, MakeLower)
{
  const auto source =
    "int main()"
    "{"
    "    string s = "
    "\"qwertyuiopasdfghjklzxcvbnm01234567890QWERTYUIOPASDFGHJKLZXCVBNM\";"
    "    string s2 = s.make_lower();"
    "    bool test1 = s == "
    "\"qwertyuiopasdfghjklzxcvbnm01234567890QWERTYUIOPASDFGHJKLZXCVBNM\";"
    "    bool test2 = s2 == "
    "\"qwertyuiopasdfghjklzxcvbnm01234567890qwertyuiopasdfghjklzxcvbnm\";"
    "    return int(test1 && test2);"
    "}";
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(1));
}

// Todo: Add more test cases
TEST_F(StringTypeSmokeTest, Upper)
{
  const auto source =
    "int main()"
    "{"
    "    string s = "
    "\"qwertyuiopasdfghjklzxcvbnm01234567890QWERTYUIOPASDFGHJKLZXCVBNM\";"
    "    s.upper();"
    "    return int(s == "
    "\"QWERTYUIOPASDFGHJKLZXCVBNM01234567890QWERTYUIOPASDFGHJKLZXCVBNM\");"
    "}";
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(1));
}

// Todo: Add more test cases
TEST_F(StringTypeSmokeTest, MakeUpper)
{
  const auto source =
    "int main()"
    "{"
    "    string s = "
    "\"qwertyuiopasdfghjklzxcvbnm01234567890QWERTYUIOPASDFGHJKLZXCVBNM\";"
    "    string s2 = s.make_upper();"
    "    bool test1 = s == "
    "\"qwertyuiopasdfghjklzxcvbnm01234567890QWERTYUIOPASDFGHJKLZXCVBNM\";"
    "    bool test2 = s2 == "
    "\"QWERTYUIOPASDFGHJKLZXCVBNM01234567890QWERTYUIOPASDFGHJKLZXCVBNM\";"
    "    return int(test1 && test2);"
    "}";
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(1));
}
}
