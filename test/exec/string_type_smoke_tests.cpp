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
        }
    }
}
