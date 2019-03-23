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

            class ReferenceTypeSmokeTest : public ::testing::Test
            {
            protected:
                cmake_facade_mock m_facade;
                source_executor m_executor{m_facade};
            };

            TEST_F(ReferenceTypeSmokeTest, DefaultConstructor)
            {
                const auto source =
                        "void foo(int& ref)\n"
                        "{\n"
                        "    ref = 42;\n"
                        "}\n"
                        "\n"
                        "int main()\n"
                        "{\n"
                        "    int i;\n"
                        "    int& i_ref = i;\n"
                        "    bool result = true;\n"
                        "    result = result && i == 0 && i_ref == 0;\n"
                        "    i = 42;\n"
                        "    result = result && i == 42 && i_ref == 42;\n"
                        "    i_ref = 24;\n"
                        "    result = result && i == 24 && i_ref == 24;\n"
                        "    foo(i_ref);\n"
                        "    result = result && i == 42 && i_ref == 42;\n"
                        "    return int(result);\n"
                        "}";
                const auto result = m_executor.execute2(source);
                EXPECT_THAT(result, Eq(1));
            }
        }
    }
}
